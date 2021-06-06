#include "ir.hpp"
#include <Arduino.h>

void IR::send_byte(uint8_t b)
{
    for (int i = 0; i < 8; ++i)
    {
        digitalWrite(pin, lo);
        delayMicroseconds(
            b & 1 ? pulse1 : pulse0);
        digitalWrite(pin, hi);
        delayMicroseconds(pulse_hi);
        b >>= 1;
    }
}
void IR::blanking_pulse()
{
    digitalWrite(pin, lo);
    delayMicroseconds(blank+pulse1);
    digitalWrite(pin, hi);
    delayMicroseconds(blank);
    digitalWrite(pin, lo);
    delayMicroseconds(blank-pulse0);
}
void IR::send_once()
{
    blanking_pulse();
    send_byte(head);
    send_byte(~head);
    send_byte(~a);
    send_byte(a);
    send_byte(b);
    send_byte(~b);
    digitalWrite(pin, lo);
}
IR::IR(uint8_t pin, bool invert)
{
    IR::pin = pin;
    head = 0xb2;
    if(invert){
        lo=HIGH;
        hi=LOW;
    } else {
        lo=LOW;
        hi=HIGH;
    }    
    pinMode(pin, OUTPUT);
    digitalWrite(pin, lo);
}
IR::IR(void)
{
    IR(PC13, true);
}
void IR::tx(void)
{
    send_once();
    send_once();
}
void IR::tx(uint16_t msg)
{
    b = msg;
    a = msg >> 8;
    tx();
}