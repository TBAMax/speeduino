
#include <Arduino.h>
#include <unity.h>

#include "scheduler.h"

#define TIMEOUT 1000
#define DURATION 1000

static void emptyCallback(void) {  }

void test_status_off_to_pending_inj1(void)
{
    initialiseSchedulers();
    setFuelSchedule1(TIMEOUT, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule1))
}

void test_status_off_to_pending_inj2(void)
{
    initialiseSchedulers();
    setFuelSchedule2(TIMEOUT, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule2))
}

void test_status_off_to_pending_inj3(void)
{
    initialiseSchedulers();
    setFuelSchedule3(TIMEOUT, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule3))
}

void test_status_off_to_pending_inj4(void)
{
    initialiseSchedulers();
    setFuelSchedule4(TIMEOUT, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule4))
}

void test_status_off_to_pending_inj5(void)
{
#if INJ_CHANNELS >= 5
    initialiseSchedulers();
    setFuelSchedule5(TIMEOUT, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule5))
#endif
}

void test_status_off_to_pending_inj6(void)
{
#if INJ_CHANNELS >= 6
    initialiseSchedulers();
    setFuelSchedule6(TIMEOUT, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule6))
#endif
}

void test_status_off_to_pending_inj7(void)
{
#if INJ_CHANNELS >= 7
    initialiseSchedulers();
    setFuelSchedule7(TIMEOUT, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule7))
#endif
}

void test_status_off_to_pending_inj8(void)
{
#if INJ_CHANNELS >= 8
    initialiseSchedulers();
    setFuelSchedule8(TIMEOUT, DURATION);
    TEST_ASSERT_TRUE(isPending(fuelSchedule8))
#endif
}


void test_status_off_to_pending_ign1(void)
{
    initialiseSchedulers();
    setIgnitionSchedule1(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    TEST_ASSERT_TRUE(isPending(ignitionSchedule1))
}

void test_status_off_to_pending_ign2(void)
{
    initialiseSchedulers();
    setIgnitionSchedule2(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    TEST_ASSERT_TRUE(isPending(ignitionSchedule2))
}

void test_status_off_to_pending_ign3(void)
{
    initialiseSchedulers();
    setIgnitionSchedule3(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    TEST_ASSERT_TRUE(isPending(ignitionSchedule3))
}

void test_status_off_to_pending_ign4(void)
{
    initialiseSchedulers();
    setIgnitionSchedule4(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    TEST_ASSERT_TRUE(isPending(ignitionSchedule4))
}

void test_status_off_to_pending_ign5(void)
{
#if IGN_CHANNELS >= 5
    initialiseSchedulers();
    setIgnitionSchedule5(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    TEST_ASSERT_TRUE(isPending(ignitionSchedule5))
#endif
}

void test_status_off_to_pending_ign6(void)
{
#if INJ_CHANNELS >= 6
    initialiseSchedulers();
    setIgnitionSchedule6(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    TEST_ASSERT_TRUE(isPending(ignitionSchedule6))
#endif
}

void test_status_off_to_pending_ign7(void)
{
#if INJ_CHANNELS >= 7
    initialiseSchedulers();
    setIgnitionSchedule7(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    TEST_ASSERT_TRUE(isPending(ignitionSchedule7))
#endif
}

void test_status_off_to_pending_ign8(void)
{
#if INJ_CHANNELS >= 8
    initialiseSchedulers();
    setIgnitionSchedule8(emptyCallback, TIMEOUT, DURATION, emptyCallback);
    TEST_ASSERT_TRUE(isPending(ignitionSchedule8))
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
