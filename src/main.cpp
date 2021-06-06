/*
 * To program hold NRST and BOOT0 down, then release NRST, then BOOT0, then upload
 */

#include <Arduino.h>

void setup()
{
  pinMode(PC13, INPUT);
  Serial.begin(115200);
  pinMode(PA1, OUTPUT);
}

int i = 1;
void loop()
{
  delay(20);
  int16_t val = analogRead(A0);
  Serial.print("Value: ");
  Serial.println(val);
  analogWrite(PA1,val>>2);
}
