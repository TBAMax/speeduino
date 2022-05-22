
#include <Arduino.h>
#include <unity.h>

#include "scheduler.h"

#define TOTAL_DURATION 2500
#define DURATION 2250

static volatile uint8_t start_count=0, end_count=0;
static void startCallback(void) { ++start_count; }
static void endCallback(void) { ++end_count; }

void test_cycle_all_states_inj()
{
    initialiseSchedulers();
    start_count = 0;
    end_count = 0;

    setCallbacks(fuelSchedule1, startCallback, endCallback);   
    TEST_ASSERT_EQUAL(OFF, fuelSchedule1.Status);
    
    setFuelSchedule(&fuelSchedule1, TOTAL_DURATION, DURATION);
    TEST_ASSERT_EQUAL_UINT8(0, start_count);
    TEST_ASSERT_EQUAL_UINT8(0, end_count);
    TEST_ASSERT_EQUAL(PENDING, fuelSchedule1.Status);
    
    while(isPending(fuelSchedule1)) /*Wait*/ ;
    TEST_ASSERT_EQUAL_UINT8(1, start_count);
    TEST_ASSERT_EQUAL_UINT8(0, end_count);
    TEST_ASSERT_EQUAL(RUNNING, fuelSchedule1.Status);
    
    setFuelSchedule(&fuelSchedule1, 2*TOTAL_DURATION, DURATION);
    TEST_ASSERT_EQUAL(RUNNINGHASNEXT, fuelSchedule1.Status);

    while(isRunning(fuelSchedule1)) /*Wait*/ ;
    TEST_ASSERT_EQUAL_UINT8(1, start_count);
    TEST_ASSERT_EQUAL_UINT8(1, end_count);
    TEST_ASSERT_EQUAL(PENDING, fuelSchedule1.Status);

    while(isPending(fuelSchedule1)) /*Wait*/ ;
    TEST_ASSERT_EQUAL_UINT8(2, start_count);
    TEST_ASSERT_EQUAL_UINT8(1, end_count);
    TEST_ASSERT_EQUAL(RUNNING, fuelSchedule1.Status);

    while(isRunning(fuelSchedule1)) /*Wait*/ ;
    TEST_ASSERT_EQUAL_UINT8(2, start_count);
    TEST_ASSERT_EQUAL_UINT8(2, end_count);
    TEST_ASSERT_EQUAL(OFF, fuelSchedule1.Status);
}


void test_cycle_all_states(void)
{
    RUN_TEST(test_cycle_all_states_inj);
    //RUN_TEST(test_cycle_all_states_ign);
}