/*
 * To program hold NRST and BOOT0 down, then release NRST, then BOOT0, then upload
 */

#include <Arduino.h>
#include <Adafruit_SI5351.h>
#include <sstream>

#define DEBUG


class Synth: public Adafruit_SI5351{
	protected:
		err_t err;
		double pll_hz;
		double clk_precision;
		int log2(unsigned int x){
			int i = 0;
			while(x/=2){++i;}
			return i;
		}
	public:
		double set(double hz, double precision){
			const double crystal_hz = 25e6;
			double pll = crystal_hz * 35.5; //Just under max frequency
			double ratio = pll / hz;
			int div = ratio / 1024;
			div = log2(div);
			if(div > 7)
				div = 7;
			ratio /= 1<<div;
			int a,b,c;
			a = ratio;
			a &= ~1; // Make divider even for integer mode
			if(a < 4)
				a = 4;
			this->setupMultisynthInt(0, SI5351_PLL_A, si5351MultisynthDiv_t(a));
			this->setupRdiv(0, si5351RDiv_t(div));
#ifdef DEBUG
			Serial.print("Integer divider: ");
			Serial.println(a);
			Serial.print("Rdiv: ");
			Serial.println(div);
#endif
			pll = a * hz * (1<<div); // Recalculate PLL frequency
			pll /= crystal_hz;
			a = pll;
			c = hz / (pll*precision);
			pll -= a;
			b = c * pll;
			this->clk_precision = hz / (a * c);
#ifdef DEBUG
			Serial.print("PLL Multiplier: ");
			Serial.print(a);
			Serial.print(" + ");
			Serial.print(b);
			Serial.print(" / ");
			Serial.println(c);
			Serial.print("Precision: ");
			Serial.print(this->clk_precision);
			Serial.println(" Hz");
#endif
			this->setupPLL(SI5351_PLL_A, a, b, c);
			this->enableOutputs(true);
			return this->clk_precision;
		}
		void print_error(void){
			switch(err){
				case ERROR_NONE:
					Serial.println("OK");
					break;
				case ERROR_OPERATIONTIMEDOUT:
					Serial.println("Timeout");
					break;
				case ERROR_INVALIDPARAMETER:
					Serial.println("Bad parameter");
					break;
				case ERROR_UNEXPECTEDVALUE:
					Serial.println("Unexpected value");
					break;
				default:
					Serial.print("Other error ");
					Serial.println(err, HEX);
					break;
			}
		}
};

Synth clk;

void setup()
{
	Serial.begin(115200);
	clk.begin();
}

void loop()
{
	double hz = Serial.parseFloat();
	if( hz > 0.00235 ){
		Serial.print(hz, 6);
		Serial.print(" MHz... ");
		clk.set(hz * 1e6, 1e3);
		clk.print_error();
	} else if(hz > 0.){
		Serial.println("Minimum frequency is 0.00235 MHz");
	} else {
		// Serial timed out, retry
	}
}
