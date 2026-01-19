#ifndef DECODERS_H
#define DECODERS_H

#include "globals.h"

#ifndef UNIT_TEST 
#define TOOTH_LOG_SIZE      128U
#else
#define TOOTH_LOG_SIZE      1U
#endif
// Some code relies on TOOTH_LOG_SIZE being uint8_t.
static_assert(TOOTH_LOG_SIZE<UINT8_MAX, "Check all uses of TOOTH_LOG_SIZE");

#if defined(CORE_AVR)
  #define READ_PRI_TRIGGER() ((*triggerPri_pin_port & triggerPri_pin_mask) ? true : false)
  #define READ_SEC_TRIGGER() ((*triggerSec_pin_port & triggerSec_pin_mask) ? true : false)
  #define READ_THIRD_TRIGGER() ((*triggerThird_pin_port & triggerThird_pin_mask) ? true : false)
#else
  #define READ_PRI_TRIGGER() digitalRead(pinTrigger)
  #define READ_SEC_TRIGGER() digitalRead(pinTrigger2)
  #define READ_THIRD_TRIGGER() digitalRead(pinTrigger3)  
#endif

#define DECODER_MISSING_TOOTH     0
#define DECODER_BASIC_DISTRIBUTOR 1
#define DECODER_DUAL_WHEEL        2
#define DECODER_GM7X              3
#define DECODER_4G63              4
#define DECODER_24X               5
#define DECODER_JEEP2000          6
#define DECODER_AUDI135           7
#define DECODER_HONDA_D17         8
#define DECODER_MIATA_9905        9
#define DECODER_MAZDA_AU          10
#define DECODER_NON360            11
#define DECODER_NISSAN_360        12
#define DECODER_SUBARU_67         13
#define DECODER_DAIHATSU_PLUS1    14
#define DECODER_HARLEY            15
#define DECODER_36_2_2_2          16
#define DECODER_36_2_1            17
#define DECODER_420A              18
#define DECODER_WEBER             19
#define DECODER_ST170             20
#define DECODER_DRZ400            21
#define DECODER_NGC               22
#define DECODER_VMAX              23
#define DECODER_RENIX             24
#define DECODER_ROVERMEMS		      25
#define DECODER_SUZUKI_K6A        26
#define DECODER_HONDA_J32         27
#define DECODER_FORD_TFI          28

#define BIT_DECODER_2ND_DERIV           0 //The use of the 2nd derivative calculation is limited to certain decoders. This is set to either true or false in each decoders setup routine
#define BIT_DECODER_IS_SEQUENTIAL       1 //Whether or not the decoder supports sequential operation
#define BIT_DECODER_UNUSED1             2 
#define BIT_DECODER_HAS_SECONDARY       3 
#define BIT_DECODER_HAS_FIXED_CRANKING  4 //Whether or not the decoder supports fixed cranking timing
#define BIT_DECODER_VALID_TRIGGER       5 //Is set true when the last trigger (Primary or secondary) was valid (ie passed filters)
#define BIT_DECODER_TOOTH_ANG_CORRECT   6 //Whether or not the triggerToothAngle variable is currently accurate. Some patterns have times when the triggerToothAngle variable cannot be accurately set.

#define TRIGGER_FILTER_OFF              0
#define TRIGGER_FILTER_LITE             1
#define TRIGGER_FILTER_MEDIUM           2
#define TRIGGER_FILTER_AGGRESSIVE       3

typedef void (*voidVoidCallback)(void);

extern uint32_t toothHistory[TOOTH_LOG_SIZE];
extern volatile unsigned int toothHistoryIndex;
extern volatile uint8_t decoderState;
extern unsigned long MAX_STALL_TIME;


//static void nullTriggerHandler(void);

//64 element LIFO circular buffer, holding uint32_t values
class CircularBuffer {
public:
    CircularBuffer(uint32_t *buffer_ptr)
        : buffer(buffer_ptr)
    {}
    //  Advance the head and push new value (overwrite oldest)
    void push(uint16_t v) {
        //head = (head + 1) & 0x3F;  //bitwise AND is used for wrap-around at 64
        head = (head + 1) & 0x7F;  //bitwise AND is used for wrap-around at 128
        buffer[head] = v;
    }

    // Update newest element WITHOUT advancing the buffer
    void updateLast(uint16_t v) {
        buffer[head] = v;
    }

    // Get element relative to head.
    // pos = 0 → newest
    // pos = -1 → previous
    // pos = -2 → two behind newest, etc.
    // pos can be max 64 (positive or negative).
    uint32_t try_get(int8_t relativePos) {
        uint8_t idx = (head + relativePos) & 0x3F; //wrap-around at 64
        return buffer[idx];
    }

    uint32_t getLast() {
        return buffer[head];
    }

private:
    uint8_t   head;
    //uint8_t   tail;
    uint32_t *buffer; // pointer to external buffer
};

struct DecoderBase{
  private:
    virtual uint16_t getGapCoeff(uint8_t toothNum) = 0; //Get the next expected time gap after the tooth, relative to the previous gap seen. expressed in 1/256ths  
    virtual uint16_t getToothAngle(uint8_t toothNum) = 0; //Get the angle of a specific tooth number
  protected:
    //setup related variables
    static uint8_t triggerActualTeeth;      //The number of physical teeth on the wheel.
    static uint8_t teethToSync; //Number of teeth matching in the pattern before we assume successful sync
    static uint8_t checkSyncToothCount; //How many teeth must've been seen successfully before we try to confirm sync (Useful for missing tooth type decoders)
    static uint8_t zeroDegreeTooth; //The tooth number that corresponds to 0 degrees crank angle  
    //work related variables
    static bool primarySync; //Whether or not the primary trigger is currently synced
    static bool secondarySync; //Whether or not the secondary trigger is currently synced
    static bool revolutionOne; // For sequential operation, this tracks whether the current revolution is 1 or 2. true = 1, false = 2
    static uint32_t lastGap; // The time gap (in uS) between the last 2 teeth seen
    static uint8_t toothCurrentCount; //The current number of teeth. 0-th tooth is the tooth before 0 degrees
    static uint8_t totalToothCount; //Total teeth seen since last sync, caps at 255    
  public:
    static uint8_t decoderState;
    virtual void triggerSetup(void) = 0;
    virtual void triggerPri(void);    //latin "primus" = "first"
    virtual void triggerSec(void) = 0;    //latin "secundus" = "second"
    virtual void triggerTert(void) {};    //latin "tertius" = "third"
    uint16_t getLastToothAngle(void);
    uint32_t getLastToothTime(void);
    int8_t getDecoderStatus(void);
    virtual uint16_t getRPM(void);
    virtual int getCrankAngle(void);
    virtual uint32_t getMicrosPerDegree(void) = 0;
    virtual uint16_t getDegreesPerMicros(void);
    virtual uint32_t getRotationTimeMicros(void);
    //static functions are not associtated with an instance of the class
    static bool engineIsRunning(uint32_t atLeastMicros= MAX_STALL_TIME);
    static bool engineIsStopped(uint32_t atLeastMicros= MAX_STALL_TIME);
};

struct DecoderMissingTooth1 : public DecoderBase {
    void triggerSetup(void) override;
    //void triggerPri(void) override;
    void triggerSec(void) override;
    uint16_t getGapCoeff(uint8_t toothNum) override;
    uint16_t getToothAngle(uint8_t toothNum) override;   
    uint32_t getMicrosPerDegree(void) override;
};

/*
extern volatile bool validTrigger; //Is set true when the last trigger (Primary or secondary) was valid (ie passed filters)
extern volatile bool triggerToothAngleIsCorrect; //Whether or not the triggerToothAngle variable is currently accurate. Some patterns have times when the triggerToothAngle variable cannot be accurately set.
extern bool secondDerivEnabled; //The use of the 2nd derivative calculation is limited to certain decoders. This is set to either true or false in each decoders setup routine
extern bool decoderIsSequential; //Whether or not the decoder supports sequential operation
extern bool decoderHasSecondary; //Whether or not the pattern uses a secondary input
extern bool decoderHasFixedCrankingTiming; 
*/

void loggerPrimaryISR(void);
void loggerSecondaryISR(void);
void loggerTertiaryISR(void);

//All of the below are the 6 required functions for each decoder / pattern
void triggerSetup_missingTooth(void);
void triggerPri_missingTooth(void);
void triggerSec_missingTooth(void);
void triggerThird_missingTooth(void);
uint16_t getRPM_missingTooth(void);
int getCrankAngle_missingTooth(void);
extern void triggerSetEndTeeth_missingTooth(void);


void triggerSetup_DualWheel(void);
void triggerPri_DualWheel(void);
void triggerSec_DualWheel(void);
uint16_t getRPM_DualWheel(void);
int getCrankAngle_DualWheel(void);
void triggerSetEndTeeth_DualWheel(void);

void triggerSetup_BasicDistributor(void);
void triggerPri_BasicDistributor(void);
void triggerSec_BasicDistributor(void);
uint16_t getRPM_BasicDistributor(void);
int getCrankAngle_BasicDistributor(void);
void triggerSetEndTeeth_BasicDistributor(void);

void triggerSetup_GM7X(void);
void triggerPri_GM7X(void);
void triggerSec_GM7X(void);
uint16_t getRPM_GM7X(void);
int getCrankAngle_GM7X(void);
void triggerSetEndTeeth_GM7X(void);

void triggerSetup_4G63(void);
void triggerPri_4G63(void);
void triggerSec_4G63(void);
uint16_t getRPM_4G63(void);
int getCrankAngle_4G63(void);
void triggerSetEndTeeth_4G63(void);

void triggerSetup_24X(void);
void triggerPri_24X(void);
void triggerSec_24X(void);
uint16_t getRPM_24X(void);
int getCrankAngle_24X(void);
void triggerSetEndTeeth_24X(void);

void triggerSetup_Jeep2000(void);
void triggerPri_Jeep2000(void);
void triggerSec_Jeep2000(void);
uint16_t getRPM_Jeep2000(void);
int getCrankAngle_Jeep2000(void);
void triggerSetEndTeeth_Jeep2000(void);

void triggerSetup_Audi135(void);
void triggerPri_Audi135(void);
void triggerSec_Audi135(void);
uint16_t getRPM_Audi135(void);
int getCrankAngle_Audi135(void);
void triggerSetEndTeeth_Audi135(void);

void triggerSetup_HondaD17(void);
void triggerPri_HondaD17(void);
void triggerSec_HondaD17(void);
uint16_t getRPM_HondaD17(void);
int getCrankAngle_HondaD17(void);
void triggerSetEndTeeth_HondaD17(void);

void triggerSetup_HondaJ32(void);
void triggerPri_HondaJ32(void);
void triggerSec_HondaJ32(void);
uint16_t getRPM_HondaJ32(void);
int getCrankAngle_HondaJ32(void);
void triggerSetEndTeeth_HondaJ32(void);

void triggerSetup_Miata9905(void);
void triggerPri_Miata9905(void);
void triggerSec_Miata9905(void);
uint16_t getRPM_Miata9905(void);
int getCrankAngle_Miata9905(void);
void triggerSetEndTeeth_Miata9905(void);
int getCamAngle_Miata9905(void);

void triggerSetup_MazdaAU(void);
void triggerPri_MazdaAU(void);
void triggerSec_MazdaAU(void);
uint16_t getRPM_MazdaAU(void);
int getCrankAngle_MazdaAU(void);
void triggerSetEndTeeth_MazdaAU(void);

void triggerSetup_non360(void);
void triggerPri_non360(void);
void triggerSec_non360(void);
uint16_t getRPM_non360(void);
int getCrankAngle_non360(void);
void triggerSetEndTeeth_non360(void);

void triggerSetup_Nissan360(void);
void triggerPri_Nissan360(void);
void triggerSec_Nissan360(void);
uint16_t getRPM_Nissan360(void);
int getCrankAngle_Nissan360(void);
void triggerSetEndTeeth_Nissan360(void);

void triggerSetup_Subaru67(void);
void triggerPri_Subaru67(void);
void triggerSec_Subaru67(void);
uint16_t getRPM_Subaru67(void);
int getCrankAngle_Subaru67(void);
void triggerSetEndTeeth_Subaru67(void);

void triggerSetup_Daihatsu(void);
void triggerPri_Daihatsu(void);
void triggerSec_Daihatsu(void);
uint16_t getRPM_Daihatsu(void);
int getCrankAngle_Daihatsu(void);
void triggerSetEndTeeth_Daihatsu(void);

void triggerSetup_Harley(void);
void triggerPri_Harley(void);
void triggerSec_Harley(void);
uint16_t getRPM_Harley(void);
int getCrankAngle_Harley(void);
void triggerSetEndTeeth_Harley(void);

void triggerSetup_ThirtySixMinus222(void);
void triggerPri_ThirtySixMinus222(void);
void triggerSec_ThirtySixMinus222(void);
uint16_t getRPM_ThirtySixMinus222(void);
int getCrankAngle_ThirtySixMinus222(void);
void triggerSetEndTeeth_ThirtySixMinus222(void);

void triggerSetup_ThirtySixMinus21(void);
void triggerPri_ThirtySixMinus21(void);
void triggerSec_ThirtySixMinus21(void);
uint16_t getRPM_ThirtySixMinus21(void);
int getCrankAngle_ThirtySixMinus21(void);
void triggerSetEndTeeth_ThirtySixMinus21(void);

void triggerSetup_420a(void);
void triggerPri_420a(void);
void triggerSec_420a(void);
uint16_t getRPM_420a(void);
int getCrankAngle_420a(void);
void triggerSetEndTeeth_420a(void);

void triggerPri_Webber(void);
void triggerSec_Webber(void);

void triggerSetup_FordST170(void);
void triggerSec_FordST170(void);
uint16_t getRPM_FordST170(void);
int getCrankAngle_FordST170(void);
void triggerSetEndTeeth_FordST170(void);

void triggerSetup_DRZ400(void);
void triggerSec_DRZ400(void);

void triggerSetup_NGC(void);
void triggerPri_NGC(void);
void triggerSec_NGC4(void);
void triggerSec_NGC68(void);
uint16_t getRPM_NGC(void);
void triggerSetEndTeeth_NGC(void);

void triggerSetup_Renix(void);
void triggerPri_Renix(void);
void triggerSetEndTeeth_Renix(void);

void triggerSetup_RoverMEMS(void);
void triggerPri_RoverMEMS(void);
void triggerSec_RoverMEMS(void);
uint16_t getRPM_RoverMEMS(void);
int getCrankAngle_RoverMEMS(void);
void triggerSetEndTeeth_RoverMEMS(void);

void triggerSetup_Vmax(void);
void triggerPri_Vmax(void);
void triggerSec_Vmax(void);
uint16_t getRPM_Vmax(void);
int getCrankAngle_Vmax(void);
void triggerSetEndTeeth_Vmax(void);

void triggerSetup_SuzukiK6A(void);
void triggerPri_SuzukiK6A(void);
void triggerSec_SuzukiK6A(void);
uint16_t getRPM_SuzukiK6A(void);
int getCrankAngle_SuzukiK6A(void);
void triggerSetEndTeeth_SuzukiK6A(void);

void triggerSetup_FordTFI(void);
void triggerPri_FordTFI(void);
void triggerSec_FordTFI(void);
uint16_t getRPM_FordTFI(void);
int getCrankAngle_FordTFI(void);
void triggerSetEndTeeth_FordTFI(void);

/**
 * @brief This function is called when the engine is stopped, or when the engine is started. It resets the decoder state and the tooth tracking variables
 *
 * @return void
 */
void resetDecoder(void);

extern void (*triggerHandler)(void); //Pointer for the trigger function (Gets pointed to the relevant decoder)
extern void (*triggerSecondaryHandler)(void); //Pointer for the secondary trigger function (Gets pointed to the relevant decoder)
extern void (*triggerTertiaryHandler)(void); //Pointer for the tertiary trigger function (Gets pointed to the relevant decoder)

extern uint16_t (*getRPM)(void); //Pointer to the getRPM function (Gets pointed to the relevant decoder)
extern int (*getCrankAngle)(void); //Pointer to the getCrank Angle function (Gets pointed to the relevant decoder)
extern void (*triggerSetEndTeeth)(void); //Pointer to the triggerSetEndTeeth function of each decoder

extern volatile unsigned long curGap;
extern volatile unsigned long curTime2;
extern volatile unsigned long curGap2;
extern volatile unsigned long lastGap;
extern volatile unsigned long targetGap;

extern volatile uint16_t toothCurrentCount; //The current number of teeth (Once sync has been achieved, this can never actually be 0
extern volatile unsigned long toothLastToothTime; //The time (micros()) that the last tooth was registered
extern volatile int16_t toothLastToothAngle; //The crank angle of the last tooth 'seen'
extern volatile unsigned long toothSystemLastToothTime; //As below, but used for decoders where not every tooth count is used for calculation
extern volatile unsigned long toothLastThirdToothTime; //The time (micros()) that the last tooth was registered on the second cam input
extern volatile unsigned long toothLastMinusOneToothTime; //The time (micros()) that the tooth before the last tooth was registered
extern volatile unsigned long toothLastMinusOneSecToothTime; //The time (micros()) that the tooth before the last tooth was registered on secondary input
extern volatile unsigned long targetGap2;

extern volatile unsigned long toothOneTime; //The time (micros()) that tooth 1 last triggered
extern volatile unsigned long toothOneMinusOneTime; //The 2nd to last time (micros()) that tooth 1 last triggered
extern volatile bool revolutionOne; // For sequential operation, this tracks whether the current revolution is 1 or 2 (not 1)

extern volatile unsigned long secondaryLastToothTime; //The time (micros()) that the last tooth was registered (Cam input)
extern volatile unsigned long secondaryLastToothTime1; //The time (micros()) that the last tooth was registered (Cam input)

extern uint16_t triggerActualTeeth;
extern volatile unsigned long triggerFilterTime; // The shortest time (in uS) that pulses will be accepted (Used for debounce filtering)
extern volatile unsigned long triggerSecFilterTime; // The shortest time (in uS) that pulses will be accepted (Used for debounce filtering) for the secondary input
extern unsigned int triggerSecFilterTime_duration; // The shortest valid time (in uS) pulse DURATION
extern volatile uint16_t triggerToothAngle; //The number of crank degrees that elapse per tooth
extern byte checkSyncToothCount; //How many teeth must've been seen on this revolution before we try to confirm sync (Useful for missing tooth type decoders)
extern unsigned long elapsedTime;
extern unsigned long lastCrankAngleCalc;
extern unsigned long lastVVTtime; //The time between the vvt reference pulse and the last crank pulse

extern uint16_t ignition1EndTooth;
extern uint16_t ignition2EndTooth;
extern uint16_t ignition3EndTooth;
extern uint16_t ignition4EndTooth;
extern uint16_t ignition5EndTooth;
extern uint16_t ignition6EndTooth;
extern uint16_t ignition7EndTooth;
extern uint16_t ignition8EndTooth;


#define CRANK_SPEED 0U
#define CAM_SPEED   1U

#define TOOTH_CRANK 0
#define TOOTH_CAM_SECONDARY 1
#define TOOTH_CAM_TERTIARY  2

// used by the ROVER MEMS pattern
#define ID_TOOTH_PATTERN 0 // have we identified teeth to skip for calculating RPM?
#define SKIP_TOOTH1 1
#define SKIP_TOOTH2 2
#define SKIP_TOOTH3 3
#define SKIP_TOOTH4 4

#endif
