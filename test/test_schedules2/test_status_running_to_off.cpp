
#include <Arduino.h>
#include <unity.h>

#include "scheduler.h"

#define DURATION 1000UL

static volatile uint8_t start_count=0, end_count=0;
static void startCallback(void) { ++start_count; }
static void endCallback(void) { ++end_count; }

void test_status_running_to_off(Schedule *pSchedule)
{
    pSchedule->reset();
    start_count = 0;
    end_count = 0;
    pSchedule->setCallbacks(startCallback, endCallback);   
    pSchedule->runAction(DURATION);
    TEST_ASSERT_EQUAL_UINT8(1, start_count);
    TEST_ASSERT_EQUAL_UINT8(0, end_count);
    while( pSchedule->isPending() || pSchedule->isRunning() ) /*Wait*/ ;
    TEST_ASSERT_EQUAL_UINT8(1, start_count);
    TEST_ASSERT_EQUAL_UINT8(1, end_count);
    TEST_ASSERT_EQUAL(OFF, pSchedule->Status);
}


void test_status_running_to_off_inj1(void)
{
    test_status_running_to_off(&fuelSchedule1);
}

void test_status_running_to_off_inj2(void)
{
    test_status_running_to_off(&fuelSchedule2);
}

void test_status_running_to_off_inj3(void)
{
    test_status_running_to_off(&fuelSchedule3);
}

void test_status_running_to_off_inj4(void)
{
    test_status_running_to_off(&fuelSchedule4);
}

void test_status_running_to_off_inj5(void)
{
#if INJ_CHANNELS >= 5
    test_status_running_to_off(&fuelSchedule5);
#endif
}

void test_status_running_to_off_inj6(void)
{
#if INJ_CHANNELS >= 6
    test_status_running_to_off(&fuelSchedule6);
#endif
}

void test_status_running_to_off_inj7(void)
{
#if INJ_CHANNELS >= 7
    test_status_running_to_off(&fuelSchedule7);
#endif
}

void test_status_running_to_off_inj8(void)
{
#if INJ_CHANNELS >= 8
    test_status_running_to_off(&fuelSchedule8);
#endif
}

void test_status_running_to_off_ign1(void)
{
    test_status_running_to_off(&ignitionSchedule1);
}

void test_status_running_to_off_ign2(void)
{
    test_status_running_to_off(&ignitionSchedule2);
}

void test_status_running_to_off_ign3(void)
{
    test_status_running_to_off(&ignitionSchedule3);
}

void test_status_running_to_off_ign4(void)
{
    test_status_running_to_off(&ignitionSchedule4);
}

void test_status_running_to_off_ign5(void)
{
#if IGN_CHANNELS >= 5
    test_status_running_to_off(&ignitionSchedule5);
#endif
}

void test_status_running_to_off_ign6(void)
{
#if IGN_CHANNELS >= 6
    test_status_running_to_off(&ignitionSchedule6);
#endif
}

void test_status_running_to_off_ign7(void)
{
#if IGN_CHANNELS >= 7
    test_status_running_to_off(&ignitionSchedule7);
#endif
}

void test_status_running_to_off_ign8(void)
{
#if IGN_CHANNELS >= 8
    test_status_running_to_off(&ignitionSchedule8);
#endif
}

void test_status_running_to_off(void)
{
    RUN_TEST(test_status_running_to_off_inj1);
    RUN_TEST(test_status_running_to_off_inj2);
    RUN_TEST(test_status_running_to_off_inj3);
    RUN_TEST(test_status_running_to_off_inj4);
    RUN_TEST(test_status_running_to_off_inj5);
    RUN_TEST(test_status_running_to_off_inj6);
    RUN_TEST(test_status_running_to_off_inj7);
    RUN_TEST(test_status_running_to_off_inj8);

    RUN_TEST(test_status_running_to_off_ign1);
    RUN_TEST(test_status_running_to_off_ign2);
    RUN_TEST(test_status_running_to_off_ign3);
    RUN_TEST(test_status_running_to_off_ign4);
    RUN_TEST(test_status_running_to_off_ign5);
    RUN_TEST(test_status_running_to_off_ign6);
    RUN_TEST(test_status_running_to_off_ign7);
    RUN_TEST(test_status_running_to_off_ign8);
}
