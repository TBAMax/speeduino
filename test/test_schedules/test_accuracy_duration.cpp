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
    runSchedule(&fuelSchedule1, DURATION);
    while(isPending(fuelSchedule1)) /*Wait*/ ;
    start_time = micros();
    while(isRunning(fuelSchedule1)) /*Wait*/ ;
    end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
}

void test_accuracy_duration_inj2(void)
{
    initialiseSchedulers();
    runSchedule(&fuelSchedule2, DURATION);
    while(isPending(fuelSchedule2)) /*Wait*/ ;
    start_time = micros();
    while(isRunning(fuelSchedule2)) /*Wait*/ ;
    end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
}

void test_accuracy_duration_inj3(void)
{
    initialiseSchedulers();
    runSchedule(&fuelSchedule3, DURATION);
    while(isPending(fuelSchedule3)) /*Wait*/ ;
    start_time = micros();
    while(isRunning(fuelSchedule3)) /*Wait*/ ;
    end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
}

void test_accuracy_duration_inj4(void)
{
    initialiseSchedulers();
    runSchedule(&fuelSchedule4, DURATION);
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
    runSchedule(&fuelSchedule5, DURATION);
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
    runSchedule(&fuelSchedule6, DURATION);
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
    runSchedule(&fuelSchedule7, DURATION);
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
    runSchedule(&fuelSchedule8, DURATION);
    while(isPending(fuelSchedule8)) /*Wait*/ ;
    start_time = micros();
    while(isRunning(fuelSchedule8)) /*Wait*/ ;
    end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
#endif
}

void test_accuracy_duration_inj(IgnSchedule *pIgnSchedule)
{
    initialiseSchedulers();
    setCallbacks(*pIgnSchedule, startCallback, endCallback);
    setIgnitionSchedule(pIgnSchedule, TIMEOUT, DURATION);
    while( (isPending(*pIgnSchedule)) || isRunning(*pIgnSchedule) ) /*Wait*/ ;
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
}

void test_accuracy_duration_ign1(void)
{
    test_accuracy_duration_inj(&ignitionSchedule1);
}

void test_accuracy_duration_ign2(void)
{
    test_accuracy_duration_inj(&ignitionSchedule2);
}

void test_accuracy_duration_ign3(void)
{
    test_accuracy_duration_inj(&ignitionSchedule3);
}

void test_accuracy_duration_ign4(void)
{
    test_accuracy_duration_inj(&ignitionSchedule4);
}

void test_accuracy_duration_ign5(void)
{
#if IGN_CHANNELS >= 5
    test_accuracy_duration_inj(&ignitionSchedule5);
#endif
}

void test_accuracy_duration_ign6(void)
{
#if INJ_CHANNELS >= 6
    test_accuracy_duration_inj(&ignitionSchedule6);
#endif
}

void test_accuracy_duration_ign7(void)
{
#if INJ_CHANNELS >= 7
    test_accuracy_duration_inj(&ignitionSchedule7);
#endif
}

void test_accuracy_duration_ign8(void)
{
#if INJ_CHANNELS >= 8
    test_accuracy_duration_inj(&ignitionSchedule8);
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
