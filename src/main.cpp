/*
 * To program hold NRST and BOOT0 down, then release NRST, then BOOT0, then upload
 */

#include <Arduino.h>
#include "ir.hpp"

void setup()
{
  pinMode(PC13, OUTPUT);
}
IR remote(PC15, false);

void loop()
{
  digitalWrite(PC13, LOW);
  remote.tx(0x4040);
  digitalWrite(PC13, HIGH);
  delay(1000);
}
