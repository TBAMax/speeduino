#include <Arduino.h>
#include <unity.h>

#include "scheduler.h"

#define TIMEOUT 1000
#define DURATION 1000

static void emptyCallback(void) {  }

void test_status_off_to_pending_inj1(void)
{
    initialiseSchedulers();
    setFuelSchedule(&fuelSchedule1, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule1));
}

void test_status_off_to_pending_inj2(void)
{
    initialiseSchedulers();
    setFuelSchedule(&fuelSchedule2, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule2));
}

void test_status_off_to_pending_inj3(void)
{
    initialiseSchedulers();
    setFuelSchedule(&fuelSchedule3, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule3));
}

void test_status_off_to_pending_inj4(void)
{
    initialiseSchedulers();
    setFuelSchedule(&fuelSchedule4, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule4));
}

void test_status_off_to_pending_inj5(void)
{
#if INJ_CHANNELS >= 5
    initialiseSchedulers();
    setFuelSchedule(&fuelSchedule5, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule5))
#endif
}

void test_status_off_to_pending_inj6(void)
{
#if INJ_CHANNELS >= 6
    initialiseSchedulers();
    setFuelSchedule(&fuelSchedule6, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule6))
#endif
}

void test_status_off_to_pending_inj7(void)
{
#if INJ_CHANNELS >= 7
    initialiseSchedulers();
    setFuelSchedule(&fuelSchedule7, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule7))
#endif
}

void test_status_off_to_pending_inj8(void)
{
#if INJ_CHANNELS >= 8
    initialiseSchedulers();
    setFuelSchedule(&fuelSchedule8, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule8))
#endif
}

void test_status_off_to_pending_ign(IgnSchedule *pSchedule)
{
    initialiseSchedulers();
    setIgnitionSchedule(pSchedule, TIMEOUT, DURATION);
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
