
#include <Arduino.h>
#include <unity.h>

#include "scheduler.h"

#define TIMEOUT 1000
#define DURATION 1000

static void emptyCallback(void) {  }

void test_status_pending_to_running_inj1(void)
{
    initialiseSchedulers();
    setFuelSchedule1(TIMEOUT, DURATION);
    while(isPending(fuelSchedule1)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(fuelSchedule1));
}

void test_status_pending_to_running_inj2(void)
{
    initialiseSchedulers();
    setFuelSchedule2(TIMEOUT, DURATION);
    while(isPending(fuelSchedule2)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(fuelSchedule2));
}

void test_status_pending_to_running_inj3(void)
{
    initialiseSchedulers();
    setFuelSchedule3(TIMEOUT, DURATION);
    while(isPending(fuelSchedule3)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(fuelSchedule3));
}

void test_status_pending_to_running_inj4(void)
{
    initialiseSchedulers();
    setFuelSchedule4(TIMEOUT, DURATION);
    while(isPending(fuelSchedule4)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(fuelSchedule4));
}

void test_status_pending_to_running_inj5(void)
{
#if INJ_CHANNELS >= 5
    initialiseSchedulers();
    setFuelSchedule5(TIMEOUT, DURATION);
    while(isPending(fuelSchedule5)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(fuelSchedule5));
#endif
}

void test_status_pending_to_running_inj6(void)
{
#if INJ_CHANNELS >= 6
    initialiseSchedulers();
    setFuelSchedule6(TIMEOUT, DURATION);
    while(isPending(fuelSchedule6)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(fuelSchedule6));
#endif
}

void test_status_pending_to_running_inj7(void)
{
#if INJ_CHANNELS >= 7
    initialiseSchedulers();
    setFuelSchedule7(TIMEOUT, DURATION);
    while(isPending(fuelSchedule7)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(fuelSchedule7));
#endif
}

void test_status_pending_to_running_inj8(void)
{
#if INJ_CHANNELS >= 8
    initialiseSchedulers();
    setFuelSchedule8(TIMEOUT, DURATION);
    while(isPending(fuelSchedule8)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(fuelSchedule8));
#endif
}


void test_status_pending_to_running_ign1(void)
{
    initialiseSchedulers();
    setIgnitionSchedule1(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule1)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(ignitionSchedule1));
}

void test_status_pending_to_running_ign2(void)
{
    initialiseSchedulers();
    setIgnitionSchedule2(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule2)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(ignitionSchedule2));
}

void test_status_pending_to_running_ign3(void)
{
    initialiseSchedulers();
    setIgnitionSchedule3(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule3)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(ignitionSchedule3));
}

void test_status_pending_to_running_ign4(void)
{
    initialiseSchedulers();
    setIgnitionSchedule4(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule4)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(ignitionSchedule4));
}

void test_status_pending_to_running_ign5(void)
{
#if IGN_CHANNELS >= 5
    initialiseSchedulers();
    setIgnitionSchedule5(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule5)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(ignitionSchedule5));
#endif
}

void test_status_pending_to_running_ign6(void)
{
#if INJ_CHANNELS >= 6
    initialiseSchedulers();
    setIgnitionSchedule6(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule6)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(ignitionSchedule6));
#endif
}

void test_status_pending_to_running_ign7(void)
{
#if INJ_CHANNELS >= 7
    initialiseSchedulers();
    setIgnitionSchedule7(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule7)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(ignitionSchedule7));
#endif
}

void test_status_pending_to_running_ign8(void)
{
#if INJ_CHANNELS >= 8
    initialiseSchedulers();
    setIgnitionSchedule8(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule8)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isRunning(ignitionSchedule8));
#endif
}

void test_status_pending_to_running(void)
{
    RUN_TEST(test_status_pending_to_running_inj1);
    RUN_TEST(test_status_pending_to_running_inj2);
    RUN_TEST(test_status_pending_to_running_inj3);
    RUN_TEST(test_status_pending_to_running_inj4);
    RUN_TEST(test_status_pending_to_running_inj5);
    RUN_TEST(test_status_pending_to_running_inj6);
    RUN_TEST(test_status_pending_to_running_inj7);
    RUN_TEST(test_status_pending_to_running_inj8);

    RUN_TEST(test_status_pending_to_running_ign1);
    RUN_TEST(test_status_pending_to_running_ign2);
    RUN_TEST(test_status_pending_to_running_ign3);
    RUN_TEST(test_status_pending_to_running_ign4);
    RUN_TEST(test_status_pending_to_running_ign5);
    RUN_TEST(test_status_pending_to_running_ign6);
    RUN_TEST(test_status_pending_to_running_ign7);
    RUN_TEST(test_status_pending_to_running_ign8);
}
