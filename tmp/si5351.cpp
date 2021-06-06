/*
 * si5351.c
 *
 *  Created on: Jul 7, 2020
 *      Author: marshal
 */
#include "si5351.h"

const uint8_t si5351 = 0x60; // i2c address

void pll_reset(uint8_t PLLx){
  Wire.beginTransmission(si5351);
  Wire.write(177);
  Wire.write(PLLx ? 0x80 : 0x20);
  Wire.endTransmission();
}

void synthStart(struct synth * cfg){
  Wire.beginTransmission(si5351);
  Wire.write(cfg->channel+16);
  Wire.write(cfg->PLLx ? 0x2F : 0x1F);
  Wire.endTransmission();
  pll_reset(cfg->PLLx);
}

void synthStop(struct synth * cfg){
  Wire.beginTransmission(si5351);
  Wire.write(cfg->channel+16);
  Wire.write(0x80);
  Wire.endTransmission();
}

void synthSetPhase(struct synth * cfg, float degrees){
  if(degrees < 0)
    degrees += 360; // Only allow positive delay
  //FIXME: If more than 180 degrees, we should use the clock invert feature
  // Since the delay is before the divider, we scale by the divide ratio
  degrees *= ((1./360)*(cfg->reg.synth))/(1<<25);
  cfg->phase = degrees > 127 ? 127 : degrees;
  Wire.beginTransmission(si5351);
  Wire.write(cfg->channel+165);
  Wire.write(cfg->phase);
  Wire.endTransmission();
  pll_reset(cfg->PLLx);
}

void synthWriteParam(uint8_t reg, uint64_t val, uint8_t div){
  Wire.beginTransmission(si5351);
  Wire.write(reg*8+26);
  Wire.write((uint16_t)0xFFFF); // denominator is always 0xFFFFF
  Wire.write((uint8_t)(val>>36 & 0x03) | div<<4);
  Wire.write((uint8_t)(val>>28));
  Wire.write((uint8_t)(val>>20));
  Wire.write((uint8_t)(val>>16) | 0xF0);
  Wire.write((uint8_t)(val>>8));
  Wire.write((uint8_t)val);
  Wire.endTransmission();
}

void synthWriteConfig(struct synth * cfg){
  synthWriteParam(cfg->PLLx, cfg->reg.pll, 0);
  synthWriteParam(cfg->channel+2, cfg->reg.synth, cfg->reg.divide);
}

void synthSetCarrier(struct synth * cfg, float carrier){
  // Calculate PLL and Multisynth values
  const double xtal = 27e6;
  double pll = 27; // Good starting value to put vxco at 750MHz
  double div = xtal * pll / carrier;
  double delta = div - (xtal*pll)/(carrier+1); // Linearized change per Hz
  int8_t delta_shift = round(log2(delta)); // round to nearest power of 2
  double delta2 = (0x40000000>>delta_shift)*(1.0/0x40000000U); // (Computing a negative power of 2)
  div *= delta2/delta;
  div = round(div/2)*2+1;   // Minimize chance of rollover
  pll = carrier * div / xtal; // Adjust pll to match

  // If desired frequency is very low, we need to use the additional divider register
  uint8_t div2;
  for(div2=0; div>2048; ++div2){
    div *= 0.5;
    --delta_shift;
  }

  // Store values into config
  const uint32_t fixed_shift = 1<<27;   //For shifting values to fixed-point
  cfg->reg.pll = pll * fixed_shift;
  cfg->reg.synth = div * fixed_shift;
  cfg->reg.shift = delta_shift;
  cfg->reg.divide = div2;

  synthWriteConfig(cfg);
}

void synthSetBaseband(struct synth * cfg, int32_t baseband){
  int32_t diff = baseband - cfg->baseband;
  diff >>= cfg->reg.shift;
  cfg->reg.synth -= diff;
  uint32_t val = cfg->reg.synth;
  /* This could potentially be sped up by dropping the last byte */
  Wire.beginTransmission(si5351);
  Wire.write(8*(cfg->channel)+46);
  Wire.write((uint8_t)(val>>20));
  Wire.write((uint8_t)(val>>16) | 0xF0);
  Wire.write((uint8_t)(val>>8));
  Wire.write((uint8_t)val);
  Wire.endTransmission();
}

void synthInit(struct synth * cfg, uint8_t channel, uint8_t pll){
  cfg->channel = channel;
  cfg->PLLx = pll;
}
