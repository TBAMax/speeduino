#include <Arduino.h>
#include <unity.h>

#include "scheduler.h"

#define TOTAL_DURATION 1500UL
#define DURATION 500UL


void test_status_off_to_pending_inj(FuelSchedule *pSchedule)
{
    pSchedule->reset();
    setFuelSchedule(pSchedule, TOTAL_DURATION, DURATION);
    TEST_ASSERT_TRUE(isPending(*pSchedule));
}


void test_status_off_to_pending_inj1(void)
{
    test_status_off_to_pending_inj(&fuelSchedule1);
}

void test_status_off_to_pending_inj2(void)
{
    test_status_off_to_pending_inj(&fuelSchedule2);
}

void test_status_off_to_pending_inj3(void)
{
    test_status_off_to_pending_inj(&fuelSchedule3);
}

void test_status_off_to_pending_inj4(void)
{
    test_status_off_to_pending_inj(&fuelSchedule4);
}

void test_status_off_to_pending_inj5(void)
{
#if INJ_CHANNELS >= 5
    test_status_off_to_pending_inj(&fuelSchedule5);
#endif
}

void test_status_off_to_pending_inj6(void)
{
#if INJ_CHANNELS >= 6
    test_status_off_to_pending_inj(&fuelSchedule6);
#endif
}

void test_status_off_to_pending_inj7(void)
{
#if INJ_CHANNELS >= 7
    test_status_off_to_pending_inj(&fuelSchedule7);
#endif
}

void test_status_off_to_pending_inj8(void)
{
#if INJ_CHANNELS >= 8
    test_status_off_to_pending_inj(&fuelSchedule8);
#endif
}

void test_status_off_to_pending_ign(IgnSchedule *pSchedule)
{
    pSchedule->reset();
    setIgnitionSchedule(pSchedule, TOTAL_DURATION, DURATION);
    TEST_ASSERT_TRUE(isPending(*pSchedule));
}

void test_status_off_to_pending_ign1(void)
{
    test_status_off_to_pending_ign(&ignitionSchedule1);
}

void test_status_off_to_pending_ign2(void)
{
    test_status_off_to_pending_ign(&ignitionSchedule2);
}

void test_status_off_to_pending_ign3(void)
{
    test_status_off_to_pending_ign(&ignitionSchedule3);
}

void test_status_off_to_pending_ign4(void)
{
    test_status_off_to_pending_ign(&ignitionSchedule4);
}

void test_status_off_to_pending_ign5(void)
{
#if IGN_CHANNELS >= 5
    test_status_off_to_pending_ign(&ignitionSchedule5);
#endif
}

void test_status_off_to_pending_ign6(void)
{
#if INJ_CHANNELS >= 6
    test_status_off_to_pending_ign(&ignitionSchedule6);
#endif
}

void test_status_off_to_pending_ign7(void)
{
#if INJ_CHANNELS >= 7
    test_status_off_to_pending_ign(&ignitionSchedule7);
#endif
}

void test_status_off_to_pending_ign8(void)
{
#if INJ_CHANNELS >= 8
    test_status_off_to_pending_ign(&ignitionSchedule8);
#endif
}

void test_status_off_to_pending(void)
{
    RUN_TEST(test_status_off_to_pending_inj1);
    RUN_TEST(test_status_off_to_pending_inj2);
    RUN_TEST(test_status_off_to_pending_inj3);
    RUN_TEST(test_status_off_to_pending_inj4);
    RUN_TEST(test_status_off_to_pending_inj5);
    RUN_TEST(test_status_off_to_pending_inj6);
    RUN_TEST(test_status_off_to_pending_inj7);
    RUN_TEST(test_status_off_to_pending_inj8);

    RUN_TEST(test_status_off_to_pending_ign1);
    RUN_TEST(test_status_off_to_pending_ign2);
    RUN_TEST(test_status_off_to_pending_ign3);
    RUN_TEST(test_status_off_to_pending_ign4);
    RUN_TEST(test_status_off_to_pending_ign5);
    RUN_TEST(test_status_off_to_pending_ign6);
    RUN_TEST(test_status_off_to_pending_ign7);
    RUN_TEST(test_status_off_to_pending_ign8);
}
