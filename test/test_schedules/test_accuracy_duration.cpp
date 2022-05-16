
#include <Arduino.h>
#include <unity.h>

#include "scheduler.h"

#define TIMEOUT 1000
#define DURATION 1000
#define DELTA 20

static uint32_t start_time, end_time;
static void startCallback(void) { start_time = micros(); }
static void endCallback(void) { end_time = micros(); }

void test_accuracy_duration_inj1(void)
{
    initialiseSchedulers();
    setFuelSchedule1(TIMEOUT, DURATION);
    while(isPending(fuelSchedule1)) /*Wait*/ ;
    start_time = micros();
    while(isRunning(fuelSchedule1)) /*Wait*/ ;
    end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
}

void test_accuracy_duration_inj2(void)
{
    initialiseSchedulers();
    setFuelSchedule2(TIMEOUT, DURATION);
    while(isPending(fuelSchedule2)) /*Wait*/ ;
    start_time = micros();
    while(isRunning(fuelSchedule2)) /*Wait*/ ;
    end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
}

void test_accuracy_duration_inj3(void)
{
    initialiseSchedulers();
    setFuelSchedule3(TIMEOUT, DURATION);
    while(isPending(fuelSchedule3)) /*Wait*/ ;
    start_time = micros();
    while(isRunning(fuelSchedule3)) /*Wait*/ ;
    end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
}

void test_accuracy_duration_inj4(void)
{
    initialiseSchedulers();
    setFuelSchedule4(TIMEOUT, DURATION);
    while(isPending(fuelSchedule4)) /*Wait*/ ;
    start_time = micros();
    while(isRunning(fuelSchedule4)) /*Wait*/ ;
    end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
}

void test_accuracy_duration_inj5(void)
{
#if INJ_CHANNELS >= 5
    initialiseSchedulers();
    setFuelSchedule5(TIMEOUT, DURATION);
    while(isPending(fuelSchedule5)) /*Wait*/ ;
    start_time = micros();
    while(isRunning(fuelSchedule5)) /*Wait*/ ;
    end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
#endif
}

void test_accuracy_duration_inj6(void)
{
#if INJ_CHANNELS >= 6
    initialiseSchedulers();
    setFuelSchedule6(TIMEOUT, DURATION);
    while(isPending(fuelSchedule6)) /*Wait*/ ;
    start_time = micros();
    while(isRunning(fuelSchedule6)) /*Wait*/ ;
    end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
#endif
}

void test_accuracy_duration_inj7(void)
{
#if INJ_CHANNELS >= 7
    initialiseSchedulers();
    setFuelSchedule7(TIMEOUT, DURATION);
    while(isPending(fuelSchedule7)) /*Wait*/ ;
    start_time = micros();
    while(isRunning(fuelSchedule7)) /*Wait*/ ;
    end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
#endif
}

void test_accuracy_duration_inj8(void)
{
#if INJ_CHANNELS >= 8
    initialiseSchedulers();
    setFuelSchedule8(TIMEOUT, DURATION);
    while(isPending(fuelSchedule8)) /*Wait*/ ;
    start_time = micros();
    while(isRunning(fuelSchedule8)) /*Wait*/ ;
    end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
#endif
}



void test_accuracy_duration_ign1(void)
{
    initialiseSchedulers();
    setIgnitionSchedule1(startCallback, TIMEOUT, DURATION, endCallback);
    while( (isPending(ignitionSchedule1)) || isRunning(ignitionSchedule1) ) /*Wait*/ ;
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
}

void test_accuracy_duration_ign2(void)
{
    initialiseSchedulers();
    setIgnitionSchedule2(startCallback, TIMEOUT, DURATION, endCallback);
    while( (isPending(ignitionSchedule2)) || isRunning(ignitionSchedule2)) ) /*Wait*/ ;
    TEST_ASSERT_UINT32_WITHIN(DELTA, TIMEOUT, end_time - start_time);
}

void test_accuracy_duration_ign3(void)
{
    initialiseSchedulers();
    setIgnitionSchedule3(startCallback, TIMEOUT, DURATION, endCallback);
    while( (isPending(ignitionSchedule3)) || isRunning(ignitionSchedule3) ) /*Wait*/ ;
    TEST_ASSERT_UINT32_WITHIN(DELTA, TIMEOUT, end_time - start_time);
}

void test_accuracy_duration_ign4(void)
{
    initialiseSchedulers();
    setIgnitionSchedule4(startCallback, TIMEOUT, DURATION, endCallback);
    while( (isPending(ignitionSchedule4)) || isRunning(ignitionSchedule4)) ) /*Wait*/ ;
    TEST_ASSERT_UINT32_WITHIN(DELTA, TIMEOUT, end_time - start_time);
}

void test_accuracy_duration_ign5(void)
{
#if IGN_CHANNELS >= 5
    initialiseSchedulers();
    setIgnitionSchedule5(startCallback, TIMEOUT, DURATION, endCallback);
    while( (isPending(ignitionSchedule5)) || isRunning(ignitionSchedule5) ) /*Wait*/ ;
    TEST_ASSERT_UINT32_WITHIN(DELTA, TIMEOUT, end_time - start_time);
#endif
}

void test_accuracy_duration_ign6(void)
{
#if INJ_CHANNELS >= 6
    initialiseSchedulers();
    setIgnitionSchedule6(startCallback, TIMEOUT, DURATION, endCallback);
    while( (isPending(ignitionSchedule6)) || isRunning(ignitionSchedule6)) ) /*Wait*/ ;
    TEST_ASSERT_UINT32_WITHIN(DELTA, TIMEOUT, end_time - start_time);
#endif
}

void test_accuracy_duration_ign7(void)
{
#if INJ_CHANNELS >= 7
    initialiseSchedulers();
    setIgnitionSchedule7(startCallback, TIMEOUT, DURATION, endCallback);
    while( (isPending(ignitionSchedule7)) || isRunning(ignitionSchedule7) ) /*Wait*/ ;
    TEST_ASSERT_UINT32_WITHIN(DELTA, TIMEOUT, end_time - start_time);
#endif
}

void test_accuracy_duration_ign8(void)
{
#if INJ_CHANNELS >= 8
    initialiseSchedulers();
    setIgnitionSchedule8(startCallback, TIMEOUT, DURATION, endCallback);
    while( (isPending(ignitionSchedule8)) || isRunning(ignitionSchedule8)) ) /*Wait*/ ;
    TEST_ASSERT_UINT32_WITHIN(DELTA, TIMEOUT, end_time - start_time);
#endif
}

void test_accuracy_duration(void)
{
    RUN_TEST(test_accuracy_duration_inj1);
    RUN_TEST(test_accuracy_duration_inj2);
    RUN_TEST(test_accuracy_duration_inj3);
    RUN_TEST(test_accuracy_duration_inj4);
    RUN_TEST(test_accuracy_duration_inj5);
    RUN_TEST(test_accuracy_duration_inj6);
    RUN_TEST(test_accuracy_duration_inj7);
    RUN_TEST(test_accuracy_duration_inj8);

    RUN_TEST(test_accuracy_duration_ign1);
    RUN_TEST(test_accuracy_duration_ign2);
    RUN_TEST(test_accuracy_duration_ign3);
    RUN_TEST(test_accuracy_duration_ign4);
    RUN_TEST(test_accuracy_duration_ign5);
    RUN_TEST(test_accuracy_duration_ign6);
    RUN_TEST(test_accuracy_duration_ign7);
    RUN_TEST(test_accuracy_duration_ign8);
}
