#include <Arduino.h>
#include <unity.h>

#include "scheduler.h"
#include "scheduledIO.h"

#define TIMEOUT 1500
#define DURATION 500
#define DELTA 24

static void startCallback(void) { /*Empty*/ }
static void endCallback(void) { /*Empty*/ }

 void test_accuracy_timeout_inj(FuelSchedule *pSchedule)
{
    initialiseSchedulers();
    pSchedule->pStartFunction = startCallback;
    pSchedule->pEndFunction = endCallback;
    uint32_t start_time = micros();
    setFuelSchedule(pSchedule, TIMEOUT, DURATION);
    while(isPending(*pSchedule)) /*Wait*/ ;
    uint32_t end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, TIMEOUT - DURATION, end_time - start_time);
}

void test_accuracy_timeout_inj1(void)
{
    test_accuracy_timeout_inj(&fuelSchedule1);
}

void test_accuracy_timeout_inj2(void)
{
    test_accuracy_timeout_inj(&fuelSchedule2);
}

void test_accuracy_timeout_inj3(void)
{
    test_accuracy_timeout_inj(&fuelSchedule3);
}

void test_accuracy_timeout_inj4(void)
{
    test_accuracy_timeout_inj(&fuelSchedule4);
}

void test_accuracy_timeout_inj5(void)
{
#if INJ_CHANNELS >= 5
    test_accuracy_timeout_inj(&fuelSchedule5);
#endif
}

void test_accuracy_timeout_inj6(void)
{
#if INJ_CHANNELS >= 6
    test_accuracy_timeout_inj(&fuelSchedule6);
#endif
}

void test_accuracy_timeout_inj7(void)
{
#if INJ_CHANNELS >= 7
    test_accuracy_timeout_inj(&fuelSchedule7);
#endif
}

void test_accuracy_timeout_inj8(void)
{
#if INJ_CHANNELS >= 8
    test_accuracy_timeout_inj(&fuelSchedule8);
#endif
}

void test_accuracy_timeout_ign(IgnSchedule *pSchedule)
{
    initialiseSchedulers();
    pSchedule->pStartFunction = startCallback;
    pSchedule->pEndFunction = endCallback;    
    uint32_t start_time = micros();
    setIgnitionSchedule(pSchedule, TIMEOUT, DURATION);
    while(isPending(*pSchedule)) /*Wait*/ ;
    uint32_t end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, TIMEOUT - DURATION, end_time - start_time);
}


void test_accuracy_timeout_ign1(void)
{
    test_accuracy_timeout_ign(&ignitionSchedule1);
}

void test_accuracy_timeout_ign2(void)
{
    test_accuracy_timeout_ign(&ignitionSchedule2);
}

void test_accuracy_timeout_ign3(void)
{
    test_accuracy_timeout_ign(&ignitionSchedule3);
}

void test_accuracy_timeout_ign4(void)
{
    test_accuracy_timeout_ign(&ignitionSchedule4);
}

void test_accuracy_timeout_ign5(void)
{
#if IGN_CHANNELS >= 5
    test_accuracy_timeout_ign(&ignitionSchedule5);
#endif
}

void test_accuracy_timeout_ign6(void)
{
#if INJ_CHANNELS >= 6
    test_accuracy_timeout_ign(&ignitionSchedule6);
#endif
}

void test_accuracy_timeout_ign7(void)
{
#if INJ_CHANNELS >= 7
    test_accuracy_timeout_ign(&ignitionSchedule7);
#endif
}

void test_accuracy_timeout_ign8(void)
{
#if INJ_CHANNELS >= 8
    test_accuracy_timeout_ign(&ignitionSchedule8);
#endif
}

void test_accuracy_timeout(void)
{
    RUN_TEST(test_accuracy_timeout_inj1);
    RUN_TEST(test_accuracy_timeout_inj2);
    RUN_TEST(test_accuracy_timeout_inj3);
    RUN_TEST(test_accuracy_timeout_inj4);
    RUN_TEST(test_accuracy_timeout_inj5);
    RUN_TEST(test_accuracy_timeout_inj6);
    RUN_TEST(test_accuracy_timeout_inj7);
    RUN_TEST(test_accuracy_timeout_inj8);

    RUN_TEST(test_accuracy_timeout_ign1);
    RUN_TEST(test_accuracy_timeout_ign2);
    RUN_TEST(test_accuracy_timeout_ign3);
    RUN_TEST(test_accuracy_timeout_ign4);
    RUN_TEST(test_accuracy_timeout_ign5);
    RUN_TEST(test_accuracy_timeout_ign6);
    RUN_TEST(test_accuracy_timeout_ign7);
    RUN_TEST(test_accuracy_timeout_ign8);
}
