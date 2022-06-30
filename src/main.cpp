/*
 * To program hold NRST and BOOT0 down, then release NRST, then BOOT0, then upload
 */

#include <Arduino.h>
#include <Adafruit_SI5351.h>

Adafruit_SI5351 clk;

void setup()
{
	Serial.begin(115200);
	clk.begin();
	clk.setupPLLInt(SI5351_PLL_A, 36); // 900MHz
}

int i = 8;
void loop()
{
	clk.setupMultisynth(0, SI5351_PLL_A, i, 0, 1);
	clk.enableOutputs(true);
	Serial.println(i);
	delay(1000);
	++i;
}
