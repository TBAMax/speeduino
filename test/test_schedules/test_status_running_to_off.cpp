
#include <Arduino.h>
#include <unity.h>

#include "scheduler.h"

#define TIMEOUT 1000
#define DURATION 1000

static void emptyCallback(void) {  }

void test_status_running_to_off_inj1(void)
{
    initialiseSchedulers();
    setFuelSchedule1(TIMEOUT, DURATION);
    while( (isPending(fuelSchedule1)) || isRunning(fuelSchedule1) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, fuelSchedule1.Status);
}

void test_status_running_to_off_inj2(void)
{
    initialiseSchedulers();
    setFuelSchedule2(TIMEOUT, DURATION);
    while( (isPending(fuelSchedule2)) || isRunning(fuelSchedule2) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, fuelSchedule2.Status);
}

void test_status_running_to_off_inj3(void)
{
    initialiseSchedulers();
    setFuelSchedule3(TIMEOUT, DURATION);
    while( (isPending(fuelSchedule3)) || isRunning(fuelSchedule3) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, fuelSchedule3.Status);
}

void test_status_running_to_off_inj4(void)
{
    initialiseSchedulers();
    setFuelSchedule4(TIMEOUT, DURATION);
    while( (isPending(fuelSchedule4)) || isRunning((fuelSchedule4) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, fuelSchedule4.Status);
}

void test_status_running_to_off_inj5(void)
{
#if INJ_CHANNELS >= 5
    initialiseSchedulers();
    setFuelSchedule5(TIMEOUT, DURATION);
    while( (isPending(fuelSchedule5)) || isRunning(fuelSchedule5) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, fuelSchedule5.Status);
#endif
}

void test_status_running_to_off_inj6(void)
{
#if INJ_CHANNELS >= 6
    initialiseSchedulers();
    setFuelSchedule6(TIMEOUT, DURATION);
    while( (isPending(fuelSchedule6)) || isRunning(fuelSchedule6) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, fuelSchedule6.Status);
#endif
}

void test_status_running_to_off_inj7(void)
{
#if INJ_CHANNELS >= 7
    initialiseSchedulers();
    setFuelSchedule7(TIMEOUT, DURATION);
    while( (isPending(fuelSchedule7)) || isRunning(fuelSchedule7) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, fuelSchedule7.Status);
#endif
}

void test_status_running_to_off_inj8(void)
{
#if INJ_CHANNELS >= 8
    initialiseSchedulers();
    setFuelSchedule8(TIMEOUT, DURATION);
    while( (isPending(fuelSchedule8)) || isRunning(fuelSchedule8) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, fuelSchedule8.Status);
#endif
}


void test_status_running_to_off_ign1(void)
{
    initialiseSchedulers();
    setIgnitionSchedule1(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while( (isPending(ignitionSchedule1)) || isRunning(ignitionSchedule1) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, ignitionSchedule1.Status);
}

void test_status_running_to_off_ign2(void)
{
    initialiseSchedulers();
    setIgnitionSchedule2(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while( (isPending(ignitionSchedule2)) || isRunning(ignitionSchedule2) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, ignitionSchedule2.Status);
}

void test_status_running_to_off_ign3(void)
{
    initialiseSchedulers();
    setIgnitionSchedule3(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while( (isPending(ignitionSchedule3)) || isRunning(ignitionSchedule3) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, ignitionSchedule3.Status);
}

void test_status_running_to_off_ign4(void)
{
    initialiseSchedulers();
    setIgnitionSchedule4(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while( (isPending(ignitionSchedule4)) || isRunning(ignitionSchedule4) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, ignitionSchedule4.Status);
}

void test_status_running_to_off_ign5(void)
{
#if IGN_CHANNELS >= 5
    initialiseSchedulers();
    setIgnitionSchedule5(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while( (isPending(ignitionSchedule5)) || isRunning(ignitionSchedule5) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, ignitionSchedule5.Status);
#endif
}

void test_status_running_to_off_ign6(void)
{
#if IGN_CHANNELS >= 6
    initialiseSchedulers();
    setIgnitionSchedule6(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while( (isPending(ignitionSchedule6)) || isRunning(ignitionSchedule6) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, ignitionSchedule6.Status);
#endif
}

void test_status_running_to_off_ign7(void)
{
#if IGN_CHANNELS >= 7
    initialiseSchedulers();
    setIgnitionSchedule7(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while( (isPending(ignitionSchedule7)) || isRunning(ignitionSchedule7) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, ignitionSchedule7.Status);
#endif
}

void test_status_running_to_off_ign8(void)
{
#if IGN_CHANNELS >= 8
    initialiseSchedulers();
    setIgnitionSchedule8(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    while( (isPending(ignitionSchedule8)) || isRunning(ignitionSchedule8) ) /*Wait*/ ;
    TEST_ASSERT_EQUAL(OFF, ignitionSchedule8.Status);
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
