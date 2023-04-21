#include "TinjaTest.hpp"

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
  return runTinjaTest();
}

void setup() {
  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(2000);
  runTinjaTest();
}

void loop() {
}