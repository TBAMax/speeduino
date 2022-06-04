
#include <Arduino.h>
#include <unity.h>

#include "scheduler.h"

#define TOTAL_DURATION 2500UL
#define DURATION 2250UL

static volatile uint8_t start_count=0, end_count=0;
static void startCallback(void) { ++start_count; }
static void endCallback(void) { ++end_count; }

void test_cycle_all_states_inj()
{
    fuelSchedule1.reset();
    start_count = 0;
    end_count = 0;

    fuelSchedule1.setCallbacks(startCallback, endCallback);   
    TEST_ASSERT_EQUAL(OFF, fuelSchedule1.Status);
    
    TEST_ASSERT_EQUAL(Schedule::STARTED, fuelSchedule1.beginSchedule(TOTAL_DURATION, DURATION));
    TEST_ASSERT_EQUAL_UINT8(0, start_count);
    TEST_ASSERT_EQUAL_UINT8(0, end_count);
    TEST_ASSERT_EQUAL(PENDING, fuelSchedule1.Status);
    
    while(fuelSchedule1.isPending()) /*Wait*/ ;
    TEST_ASSERT_EQUAL_UINT8(1, start_count);
    TEST_ASSERT_EQUAL_UINT8(0, end_count);
    TEST_ASSERT_EQUAL(RUNNING, fuelSchedule1.Status);
    
    TEST_ASSERT_EQUAL(Schedule::QUEUED, fuelSchedule1.beginSchedule(2*TOTAL_DURATION, DURATION));
    TEST_ASSERT_EQUAL(RUNNINGHASNEXT, fuelSchedule1.Status);

    while(fuelSchedule1.isRunning()) /*Wait*/ ;
    TEST_ASSERT_EQUAL_UINT8(1, start_count);
    TEST_ASSERT_EQUAL_UINT8(1, end_count);
    TEST_ASSERT_EQUAL(PENDING, fuelSchedule1.Status);

    while(fuelSchedule1.isPending()) /*Wait*/ ;
    TEST_ASSERT_EQUAL_UINT8(2, start_count);
    TEST_ASSERT_EQUAL_UINT8(1, end_count);
    TEST_ASSERT_EQUAL(RUNNING, fuelSchedule1.Status);

    while(fuelSchedule1.isRunning()) /*Wait*/ ;
    TEST_ASSERT_EQUAL_UINT8(2, start_count);
    TEST_ASSERT_EQUAL_UINT8(2, end_count);
    TEST_ASSERT_EQUAL(OFF, fuelSchedule1.Status);
}


void test_cycle_all_states(void)
{
    RUN_TEST(test_cycle_all_states_inj);
    //RUN_TEST(test_cycle_all_states_ign);
}