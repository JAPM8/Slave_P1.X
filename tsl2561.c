/* 
 * File:   tsl2561.h
 * Author: K. Townsend (microBuilder.eu)
 * Modified by: Javier A. Pérez Marín
 * Obtained from: https://github.com/adafruit/TSL2561-Arduino-Library/blob/master/TSL2561.h
 * 
 */

#include "tsl2561.h"

uint32_t calculateLux(uint16_t broadband, uint16_t ir) {
  unsigned long chScale;
  unsigned long channel1;
  unsigned long channel0;

  /* Make sure the sensor isn't saturated! */
  uint16_t clipThreshold;
  clipThreshold = TSL2561_CLIPPING_13MS;
  /* Return 65536 lux if the sensor is saturated */
  if ((broadband > clipThreshold) || (ir > clipThreshold)) {
    return 65536;
  }
  /* Get the correct scale depending on the intergration time */
  chScale = TSL2561_LUX_CHSCALE_TINT0;
  chScale = chScale<<4;
  /* Scale the channel values */
  channel0 = (broadband * chScale) >> TSL2561_LUX_CHSCALE;
  channel1 = (ir * chScale) >> TSL2561_LUX_CHSCALE;
  /* Find the ratio of the channel values (Channel1/Channel0) */
  unsigned long ratio1 = 0;
  if (channel0 != 0)
    ratio1 = (channel1 << (TSL2561_LUX_RATIOSCALE + 1)) / channel0;
  /* round the ratio value */
  unsigned long ratio = (ratio1 + 1) >> 1;

  unsigned int b, m;

  if ((ratio >= 0) && (ratio <= TSL2561_LUX_K1T)) {
    b = TSL2561_LUX_B1T;
    m = TSL2561_LUX_M1T;
  } else if (ratio <= TSL2561_LUX_K2T) {
    b = TSL2561_LUX_B2T;
    m = TSL2561_LUX_M2T;
  } else if (ratio <= TSL2561_LUX_K3T) {
    b = TSL2561_LUX_B3T;
    m = TSL2561_LUX_M3T;
  } else if (ratio <= TSL2561_LUX_K4T) {
    b = TSL2561_LUX_B4T;
    m = TSL2561_LUX_M4T;
  } else if (ratio <= TSL2561_LUX_K5T) {
    b = TSL2561_LUX_B5T;
    m = TSL2561_LUX_M5T;
  } else if (ratio <= TSL2561_LUX_K6T) {
    b = TSL2561_LUX_B6T;
    m = TSL2561_LUX_M6T;
  } else if (ratio <= TSL2561_LUX_K7T) {
    b = TSL2561_LUX_B7T;
    m = TSL2561_LUX_M7T;
  } else if (ratio > TSL2561_LUX_K8T) {
    b = TSL2561_LUX_B8T;
    m = TSL2561_LUX_M8T;
  }

  unsigned long temp;
  channel0 = channel0 * b;
  channel1 = channel1 * m;

  temp = 0;
  /* Do not allow negative lux value */
  if (channel0 > channel1)
    temp = channel0 - channel1;

  /* Round lsb (2^(LUX_SCALE-1)) */
  temp += (1 << (TSL2561_LUX_LUXSCALE - 1));

  /* Strip off fractional portion */
  uint32_t lux = temp >> TSL2561_LUX_LUXSCALE;

  /* Signal I2C had no errors */
  return lux;
}