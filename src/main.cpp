/*
 * To program hold NRST and BOOT0 down, then release NRST, then BOOT0, then upload
 */

#include <Arduino.h>


class Prog{
	public:
		Prog();
		void enter(void);
		void exit(void);
		long read_sig(void);
		long read_fuse(void);
		void load(short addr, short data);
		void write(void);
		short read(short addr);
		enum cmd{
                        erase 	= 0x80,
                        wfuse 	= 0x40,
                        wlock 	= 0x20,
                        wflash 	= 0x10,
                        weeprom = 0x11,
                        rsig 	= 0x08,
                        rfuse 	= 0x04,
                        rflash 	= 0x02,
                        reeprom = 0x03,
                        nop 	= 0x00,
		};
		void load_addr(short addr) {load(0, addr);}
		void load_data(short data) {load(1, data);}
		void load_cmd(enum cmd c) {load(2, c);}
		void load_data_h(short data) {load(5, data);}
		void load_addr_h(short addr) {load(4, addr);}
};

Prog::Prog(){
	pinMode(PB0, OUTPUT);
	pinMode(PB1, OUTPUT);
	pinMode(PB2, OUTPUT);
	pinMode(PB3, OUTPUT);
	pinMode(PB4, OUTPUT);
	pinMode(PB5, OUTPUT);
	pinMode(PB6, INPUT);
	pinMode(PB7, OUTPUT);
}

void Prog::load(short action, short data){
	digitalWrite(PB1, action&1);
	digitalWrite(PB2, action&2);
	digitalWrite(PB3, action&4);
	digitalWrite(PB5, HIGH);
	pinMode(PA0, OUTPUT);
	pinMode(PA1, OUTPUT);
	pinMode(PA2, OUTPUT);
	pinMode(PA3, OUTPUT);
	pinMode(PA4, OUTPUT);
	pinMode(PA5, OUTPUT);
	pinMode(PA6, OUTPUT);
	pinMode(PA7, OUTPUT);
	digitalWrite(PA0, data&1);
	digitalWrite(PA1, data&2);
	digitalWrite(PA2, data&4);
	digitalWrite(PA3, data&8);
	digitalWrite(PA4, data&16);
	digitalWrite(PA5, data&32);
	digitalWrite(PA6, data&64);
	digitalWrite(PA7, data&128);

	// Pulse the clock to latch
	digitalWrite(PB4, HIGH);
	digitalWrite(PB4, LOW);
}

short Prog::read(short action){
	pinMode(PA0, INPUT);
	pinMode(PA1, INPUT);
	pinMode(PA2, INPUT);
	pinMode(PA3, INPUT);
	pinMode(PA4, INPUT);
	pinMode(PA5, INPUT);
	pinMode(PA6, INPUT);
	pinMode(PA7, INPUT);
	digitalWrite(PB1, action&1);
	digitalWrite(PB2, action&2);
	digitalWrite(PB3, action&4);
	digitalWrite(PB5, LOW);
	short data = 0;
	if(digitalRead(PA0))
		data |= 1;
	if(digitalRead(PA1))
		data |= 2;
	if(digitalRead(PA2))
		data |= 4;
	if(digitalRead(PA3))
		data |= 8;
	if(digitalRead(PA4))
		data |= 16;
	if(digitalRead(PA5))
		data |= 32;
	if(digitalRead(PA6))
		data |= 64;
	if(digitalRead(PA7))
		data |= 128;
	digitalWrite(PB5, HIGH);
	return data;
}

void Prog::write(void){
	digitalWrite(PB0, LOW);
	digitalWrite(PB0, HIGH);
	while(!digitalRead(PB6));
}

void Prog::enter(void){
	digitalWrite(PB0, LOW);
	digitalWrite(PB1, LOW);
	digitalWrite(PB2, LOW);
	digitalWrite(PB3, LOW);
	digitalWrite(PB7, HIGH);
	for(auto i = 0; i < 10; ++i){
		digitalWrite(PB4, HIGH);
		digitalWrite(PB4, LOW);
	}
	digitalWrite(PB7, LOW);
	delay(1);
}

void Prog::exit(void){
	load_cmd( nop);
	digitalWrite(PB7, HIGH);
}

long Prog::read_sig(void){
	load_cmd(rsig);
	long sig = 0;
	for(auto i = 2; i>=0; --i){
		sig <<= 8;
		load_addr(i);
		sig |= read(0);
	}
	return sig;
}

long Prog::read_fuse(void){
	load_cmd(rfuse);
	long fuse = 0;
	for(auto i = 6; i >= 0; i -= 2){
		fuse <<= 8;
		fuse |= read(i);
	}
	return fuse;
}

void setup()
{
  pinMode(PC13, OUTPUT);
  Serial.begin(115200);
}

Prog tiny26;

void loop()
{
  digitalWrite(PC13, LOW);
  delay(1000);
  digitalWrite(PC13, HIGH);
  long sig = tiny26.read_sig();
  Serial.print("\nDevice signature is ");
  Serial.print(sig, HEX);
  
  delay(1000);
}
