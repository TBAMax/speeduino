#include <Arduino.h>
#include <unity.h>

#include "scheduler.h"
#include "scheduledIO.h"

#define TOTAL_DURATION 1500UL
#define DURATION 500UL
#define DELTA 24


 void test_accuracy_timeout_inj(FuelSchedule *pSchedule)
{
    pSchedule->reset();
    uint32_t start_time = micros();
    TEST_ASSERT_EQUAL(Schedule::STARTED, pSchedule->beginSchedule(TOTAL_DURATION, DURATION));
    while(pSchedule->isPending()) /*Wait*/ ;
    uint32_t end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, TOTAL_DURATION - DURATION, end_time - start_time);
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
    pSchedule->reset();   
    uint32_t start_time = micros();
    TEST_ASSERT_EQUAL(Schedule::STARTED, pSchedule->beginSchedule(TOTAL_DURATION, DURATION));
    while(pSchedule->isPending()) /*Wait*/ ;
    uint32_t end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, TOTAL_DURATION - DURATION, end_time - start_time);
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
