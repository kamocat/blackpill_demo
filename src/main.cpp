/*
 * To program hold NRST and BOOT0 down, then release NRST, then BOOT0, then upload
 */

#include <Arduino.h>
#include <Adafruit_SI5351.h>

Adafruit_SI5351 clk;

int log2(unsigned int x){
	int i = 0;
	while(x/=2){++i;}
	return i;
}

err_t set_freq(double hz){
	double pll = 600E6;
	double ratio = pll / hz;
	int div = ratio / 1024;
	div = log2(div);
	ratio /= 1<<div;
	int a,b,c;
	a = ratio;
	ratio -= a;
	c = 1<<18;
	b = div * c;
	clk.setupRdiv(0, si5351RDiv_t(div));
	ASSERT_STATUS(clk.setupMultisynth(0, SI5351_PLL_A, a, b, c));
	return ERROR_NONE;
}

void print_error(err_t e){
	switch(e){
		case ERROR_NONE:
			Serial.println("OK");
			break;
		case ERROR_OPERATIONTIMEDOUT:
			Serial.print("Timeout");
			break;
		case ERROR_INVALIDPARAMETER:
			Serial.print("Bad parameter");
			break;
		case ERROR_UNEXPECTEDVALUE:
			Serial.print("Unexpected value");
			break;
		default:
			Serial.print("Other error ");
			Serial.print(e, HEX);
			break;
	}
}

void setup()
{
	Serial.begin(115200);
	clk.begin();
	clk.setupPLLInt(SI5351_PLL_A, 24); // 600MHz
}

void loop()
{
	double hz = Serial.parseFloat();
	if( hz > 0. ){
		Serial.print(hz);
		Serial.print(" MHz... ");
		err_t e = set_freq(hz * 1e6);
		print_error(e);
		clk.enableOutputs(true);
	}
}
