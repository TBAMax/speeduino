#include <Arduino.h>
#include <unity.h>
#include <init.h>

#include "test_schedules.h"

void setUp(void) {
}

void tearDown(void) {
  // clean stuff up here
}

void runUnityTests()
{
  UNITY_BEGIN(); // start unit testing

  initialiseAll(); //Run the main initialise function
  test_status_pending_to_running();
  test_status_running_to_pending();
  test_status_running_to_off();
  
  UNITY_END(); // stop unit testing
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(2000);

  runUnityTests();
}
void loop() {}