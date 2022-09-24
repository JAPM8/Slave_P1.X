# 1 "tsl2561.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:/Program Files/Microchip/MPLABX/v6.00/packs/Microchip/PIC16Fxxx_DFP/1.3.42/xc8\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "tsl2561.c" 2







# 1 "./tsl2561.h" 1
# 11 "./tsl2561.h"
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 1 3
# 13 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 3
typedef signed char int8_t;






typedef signed int int16_t;







typedef __int24 int24_t;







typedef signed long int int32_t;
# 52 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 3
typedef unsigned char uint8_t;





typedef unsigned int uint16_t;






typedef __uint24 uint24_t;






typedef unsigned long int uint32_t;
# 88 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 3
typedef signed char int_least8_t;







typedef signed int int_least16_t;
# 109 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 3
typedef __int24 int_least24_t;
# 118 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 3
typedef signed long int int_least32_t;
# 136 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 3
typedef unsigned char uint_least8_t;






typedef unsigned int uint_least16_t;
# 154 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 3
typedef __uint24 uint_least24_t;







typedef unsigned long int uint_least32_t;
# 181 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 3
typedef signed char int_fast8_t;






typedef signed int int_fast16_t;
# 200 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 3
typedef __int24 int_fast24_t;







typedef signed long int int_fast32_t;
# 224 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 3
typedef unsigned char uint_fast8_t;





typedef unsigned int uint_fast16_t;
# 240 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 3
typedef __uint24 uint_fast24_t;






typedef unsigned long int uint_fast32_t;
# 268 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 3
typedef int32_t intmax_t;
# 282 "C:\\Program Files\\Microchip\\xc8\\v2.35\\pic\\include\\c90\\stdint.h" 3
typedef uint32_t uintmax_t;






typedef int16_t intptr_t;




typedef uint16_t uintptr_t;
# 11 "./tsl2561.h" 2
# 47 "./tsl2561.h"
uint32_t calculateLux(uint16_t broadband, uint16_t ir);
# 8 "tsl2561.c" 2


uint32_t calculateLux(uint16_t broadband, uint16_t ir) {
  unsigned long chScale;
  unsigned long channel1;
  unsigned long channel0;


  uint16_t clipThreshold;
  clipThreshold = 4900;

  if ((broadband > 0xFFFF) || (ir > 0xFFFF)) {
    return 0;
  }

  chScale = (0x7517);
  chScale = chScale<<4;

  channel0 = (broadband * chScale) >> (10);
  channel1 = (ir * chScale) >> (10);

  unsigned long ratio1 = 0;
  if (channel0 != 0)
    ratio1 = (channel1 << ((9) + 1)) / channel0;

  unsigned long ratio = (ratio1 + 1) >> 1;

  unsigned int b, m;

  if ((ratio >= 0) && (ratio <= (0x0040))) {
    b = (0x01f2);
    m = (0x01be);
  } else if (ratio <= (0x0080)) {
    b = (0x0214);
    m = (0x02d1);
  } else if (ratio <= (0x00c0)) {
    b = (0x023f);
    m = (0x037b);
  } else if (ratio <= (0x0100)) {
    b = (0x0270);
    m = (0x03fe);
  } else if (ratio <= (0x0138)) {
    b = (0x016f);
    m = (0x01fc);
  } else if (ratio <= (0x019a)) {
    b = (0x00d2);
    m = (0x00fb);
  } else if (ratio <= (0x029a)) {
    b = (0x0018);
    m = (0x0012);
  } else if (ratio > (0x029a)) {
    b = (0x0000);
    m = (0x0000);
  }

  unsigned long temp;
  channel0 = channel0 * b;
  channel1 = channel1 * m;

  temp = 0;

  if (channel0 > channel1)
    temp = channel0 - channel1;


  temp += (1 << ((14) - 1));


  uint32_t lux = temp >> (14);


  return lux;
}
