
#include <Arduino.h>
#include <unity.h>

#include "scheduler.h"

#define TIMEOUT 1000
#define DURATION 1000

static void emptyCallback(void) {  }

void test_status_running_to_pending_inj1(void)
{
    initialiseSchedulers();
    setFuelSchedule1(TIMEOUT, DURATION);
    while(isPending(fuelSchedule1)) /*Wait*/ ;
    setFuelSchedule1(2*TIMEOUT, DURATION);
    while(isRunning(fuelSchedule1)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(fuelSchedule1))
}

void test_status_running_to_pending_inj2(void)
{
    initialiseSchedulers();
    setFuelSchedule2(TIMEOUT, DURATION);
    while(isPending(fuelSchedule2)) /*Wait*/ ;
    setFuelSchedule2(2*TIMEOUT, DURATION);
    while(isRunning(fuelSchedule2)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(fuelSchedule2))
}

void test_status_running_to_pending_inj3(void)
{
    initialiseSchedulers();
    setFuelSchedule3(TIMEOUT, DURATION);
    while(isPending(fuelSchedule3)) /*Wait*/ ;
    setFuelSchedule3(2*TIMEOUT, DURATION);
    while(isRunning(fuelSchedule3)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(fuelSchedule3))
}

void test_status_running_to_pending_inj4(void)
{
    initialiseSchedulers();
    setFuelSchedule4(TIMEOUT, DURATION);
    while(isPending(fuelSchedule4)) /*Wait*/ ;
    setFuelSchedule4(2*TIMEOUT, DURATION);
    while(isRunning(fuelSchedule4)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(fuelSchedule4))
}

void test_status_running_to_pending_inj5(void)
{
#if INJ_CHANNELS >= 5
    initialiseSchedulers();
    setFuelSchedule5(TIMEOUT, DURATION);
    while(isPending(fuelSchedule5)) /*Wait*/ ;
    setFuelSchedule5(2*TIMEOUT, DURATION);
    while(isRunning(fuelSchedule5)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(fuelSchedule5))
#endif
}

void test_status_running_to_pending_inj6(void)
{
#if INJ_CHANNELS >= 6
    initialiseSchedulers();
    setFuelSchedule6(TIMEOUT, DURATION);
    while(isPending(fuelSchedule6)) /*Wait*/ ;
    setFuelSchedule6(2*TIMEOUT, DURATION);
    while(isRunning(fuelSchedule6)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(fuelSchedule6))
#endif
}

void test_status_running_to_pending_inj7(void)
{
#if INJ_CHANNELS >= 7
    initialiseSchedulers();
    setFuelSchedule7(TIMEOUT, DURATION);
    while(isPending(fuelSchedule7)) /*Wait*/ ;
    setFuelSchedule7(2*TIMEOUT, DURATION);
    while(isRunning(fuelSchedule7)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(fuelSchedule7))
#endif
}

void test_status_running_to_pending_inj8(void)
{
#if INJ_CHANNELS >= 8
    initialiseSchedulers();
    setFuelSchedule8(TIMEOUT, DURATION);
    while(isPending(fuelSchedule8)) /*Wait*/ ;
    setFuelSchedule8(2*TIMEOUT, DURATION);
    while(isRunning(fuelSchedule8)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(fuelSchedule8))
#endif
}


void test_status_running_to_pending_ign1(void)
{
    initialiseSchedulers();
    setIgnitionSchedule1(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule1)) /*Wait*/ ;
    setIgnitionSchedule1(emptyCallback, 2*TIMEOUT, DURATION, emptyCallback);
    while(isRunning(ignitionSchedule1)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(ignitionSchedule1))
}

void test_status_running_to_pending_ign2(void)
{
    initialiseSchedulers();
    setIgnitionSchedule2(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule2)) /*Wait*/ ;
    setIgnitionSchedule2(emptyCallback, 2*TIMEOUT, DURATION, emptyCallback);
    while(isRunning(ignitionSchedule2)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(ignitionSchedule2))
}

void test_status_running_to_pending_ign3(void)
{
    initialiseSchedulers();
    setIgnitionSchedule3(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule3)) /*Wait*/ ;
    setIgnitionSchedule3(emptyCallback, 2*TIMEOUT, DURATION, emptyCallback);
    while(isRunning(ignitionSchedule3)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(ignitionSchedule3))
}

void test_status_running_to_pending_ign4(void)
{
    initialiseSchedulers();
    setIgnitionSchedule4(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule4)) /*Wait*/ ;
    setIgnitionSchedule4(emptyCallback, 2*TIMEOUT, DURATION, emptyCallback);
    while(isRunning(ignitionSchedule4)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(ignitionSchedule4))
}

void test_status_running_to_pending_ign5(void)
{
#if IGN_CHANNELS >= 5
    initialiseSchedulers();
    setIgnitionSchedule5(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule5)) /*Wait*/ ;
    setIgnitionSchedule5(emptyCallback, 2*TIMEOUT, DURATION, emptyCallback);
    while(isRunning(ignitionSchedule5)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(ignitionSchedule5))
#endif
}

void test_status_running_to_pending_ign6(void)
{
#if INJ_CHANNELS >= 6
    initialiseSchedulers();
    setIgnitionSchedule6(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule6)) /*Wait*/ ;
    setIgnitionSchedule6(emptyCallback, 2*TIMEOUT, DURATION, emptyCallback);
    while(isRunning(ignitionSchedule6)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(ignitionSchedule6))
#endif
}

void test_status_running_to_pending_ign7(void)
{
#if INJ_CHANNELS >= 7
    initialiseSchedulers();
    setIgnitionSchedule7(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule7)) /*Wait*/ ;
    setIgnitionSchedule7(emptyCallback, 2*TIMEOUT, DURATION, emptyCallback);
    while(isRunning(ignitionSchedule7)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(ignitionSchedule7))
#endif
}

void test_status_running_to_pending_ign8(void)
{
#if INJ_CHANNELS >= 8
    initialiseSchedulers();
    setIgnitionSchedule8(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while(isPending(ignitionSchedule8)) /*Wait*/ ;
    setIgnitionSchedule8(emptyCallback, 2*TIMEOUT, DURATION, emptyCallback);
    while(isRunning(ignitionSchedule8)) /*Wait*/ ;
    TEST_ASSERT_TRUE(isPending(ignitionSchedule8))
#endif
}

void test_status_running_to_pending(void)
{
    RUN_TEST(test_status_running_to_pending_inj1);
    RUN_TEST(test_status_running_to_pending_inj2);
    RUN_TEST(test_status_running_to_pending_inj3);
    RUN_TEST(test_status_running_to_pending_inj4);
    RUN_TEST(test_status_running_to_pending_inj5);
    RUN_TEST(test_status_running_to_pending_inj6);
    RUN_TEST(test_status_running_to_pending_inj7);
    RUN_TEST(test_status_running_to_pending_inj8);

    RUN_TEST(test_status_running_to_pending_ign1);
    RUN_TEST(test_status_running_to_pending_ign2);
    RUN_TEST(test_status_running_to_pending_ign3);
    RUN_TEST(test_status_running_to_pending_ign4);
    RUN_TEST(test_status_running_to_pending_ign5);
    RUN_TEST(test_status_running_to_pending_ign6);
    RUN_TEST(test_status_running_to_pending_ign7);
    RUN_TEST(test_status_running_to_pending_ign8);
}
