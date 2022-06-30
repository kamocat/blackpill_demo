/*
 * To program hold NRST and BOOT0 down, then release NRST, then BOOT0, then upload
 */

#include <Arduino.h>
#include <Adafruit_SI5351.h>

Adafruit_SI5351 clk;

void set_freq(double hz){
	double pll = 900E6;
	double div = pll / hz;
	int a,b,c;
	a = div;
	div -= a;
	c = 1<<18;
	b = div * c;
	clk.setupMultisynth(0, SI5351_PLL_A, a, b, c);
}
	

void setup()
{
	Serial.begin(115200);
	clk.begin();
	clk.setupPLLInt(SI5351_PLL_A, 36); // 900MHz
}

double i = 1E6;
void loop()
{
	set_freq(i);
	clk.enableOutputs(true);
	Serial.println(i);
	delay(1000);
	i += 1000;
}
