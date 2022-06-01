#include <Arduino.h>
#include <unity.h>

#include "scheduler.h"

#define DURATION 1000UL
#define TOTAL_DURATION 1000UL
#define DELTA 20

void test_accuracy_duration(Schedule *pSchedule)
{
    pSchedule->reset();
    pSchedule->runSchedule(DURATION);
    uint32_t start_time = micros();
    while(pSchedule->isRunning()) /*Wait*/ ;
    uint32_t end_time = micros();
    TEST_ASSERT_UINT32_WITHIN(DELTA, DURATION, end_time - start_time);
}

void test_accuracy_duration_inj1(void)
{
   test_accuracy_duration(&fuelSchedule1);
}

void test_accuracy_duration_inj2(void)
{
   test_accuracy_duration(&fuelSchedule2);
}

void test_accuracy_duration_inj3(void)
{
   test_accuracy_duration(&fuelSchedule3);
}

void test_accuracy_duration_inj4(void)
{
   test_accuracy_duration(&fuelSchedule4);
}

void test_accuracy_duration_inj5(void)
{
#if INJ_CHANNELS >= 5
   test_accuracy_duration(&fuelSchedule5);
#endif
}

void test_accuracy_duration_inj6(void)
{
#if INJ_CHANNELS >= 6
   test_accuracy_duration(&fuelSchedule6);
#endif
}

void test_accuracy_duration_inj7(void)
{
#if INJ_CHANNELS >= 7
   test_accuracy_duration(&fuelSchedule7);
#endif
}

void test_accuracy_duration_inj8(void)
{
#if INJ_CHANNELS >= 8
   test_accuracy_duration(&fuelSchedule8);
#endif
}


void test_accuracy_duration_ign1(void)
{
    test_accuracy_duration(&ignitionSchedule1);
}

void test_accuracy_duration_ign2(void)
{
    test_accuracy_duration(&ignitionSchedule2);
}

void test_accuracy_duration_ign3(void)
{
    test_accuracy_duration(&ignitionSchedule3);
}

void test_accuracy_duration_ign4(void)
{
    test_accuracy_duration(&ignitionSchedule4);
}

void test_accuracy_duration_ign5(void)
{
#if IGN_CHANNELS >= 5
    test_accuracy_duration(&ignitionSchedule5);
#endif
}

void test_accuracy_duration_ign6(void)
{
#if INJ_CHANNELS >= 6
    test_accuracy_duration(&ignitionSchedule6);
#endif
}

void test_accuracy_duration_ign7(void)
{
#if INJ_CHANNELS >= 7
    test_accuracy_duration(&ignitionSchedule7);
#endif
}

void test_accuracy_duration_ign8(void)
{
#if INJ_CHANNELS >= 8
    test_accuracy_duration(&ignitionSchedule8);
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
