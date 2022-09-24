/* 
 * File:   tsl2561.h
 * Author: K. Townsend (microBuilder.eu)
 * Modified by: Javier A. Pérez Marín
 * Obtained from: https://github.com/adafruit/TSL2561-Arduino-Library/blob/master/TSL2561.h
 * 
 */

#ifndef TSL2561_H
#define	TSL2561_H
#include <stdint.h>
#include <xc.h>

#define TSL2561_LUX_LUXSCALE (14)          ///< Scale by 2^14
#define TSL2561_LUX_RATIOSCALE (9)         ///< Scale ratio by 2^9
#define TSL2561_LUX_CHSCALE (10)           ///< Scale channel values by 2^10
#define TSL2561_LUX_CHSCALE_TINT0 (0x7517) ///< 322/11 * 2^TSL2561_LUX_CHSCALE

// T, FN and CL package values
#define TSL2561_LUX_K1T (0x0040) ///< 0.125 * 2^RATIO_SCALE
#define TSL2561_LUX_B1T (0x01f2) ///< 0.0304 * 2^LUX_SCALE 
#define TSL2561_LUX_M1T (0x01be) ///< 0.0272 * 2^LUX_SCALE
#define TSL2561_LUX_K2T (0x0080) ///< 0.250 * 2^RATIO_SCALE
#define TSL2561_LUX_B2T (0x0214) ///< 0.0325 * 2^LUX_SCALE
#define TSL2561_LUX_M2T (0x02d1) ///< 0.0440 * 2^LUX_SCALE
#define TSL2561_LUX_K3T (0x00c0) ///< 0.375 * 2^RATIO_SCALE
#define TSL2561_LUX_B3T (0x023f) ///< 0.0351 * 2^LUX_SCALE
#define TSL2561_LUX_M3T (0x037b) ///< 0.0544 * 2^LUX_SCALE
#define TSL2561_LUX_K4T (0x0100) ///< 0.50 * 2^RATIO_SCALE
#define TSL2561_LUX_B4T (0x0270) ///< 0.0381 * 2^LUX_SCALE
#define TSL2561_LUX_M4T (0x03fe) ///< 0.0624 * 2^LUX_SCALE
#define TSL2561_LUX_K5T (0x0138) ///< 0.61 * 2^RATIO_SCALE
#define TSL2561_LUX_B5T (0x016f) ///< 0.0224 * 2^LUX_SCALE
#define TSL2561_LUX_M5T (0x01fc) ///< 0.0310 * 2^LUX_SCALE
#define TSL2561_LUX_K6T (0x019a) ///< 0.80 * 2^RATIO_SCALE
#define TSL2561_LUX_B6T (0x00d2) ///< 0.0128 * 2^LUX_SCALE
#define TSL2561_LUX_M6T (0x00fb) ///< 0.0153 * 2^LUX_SCALE
#define TSL2561_LUX_K7T (0x029a) ///< 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B7T (0x0018) ///< 0.00146 * 2^LUX_SCALE
#define TSL2561_LUX_M7T (0x0012) ///< 0.00112 * 2^LUX_SCALE
#define TSL2561_LUX_K8T (0x029a) ///< 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B8T (0x0000) ///< 0.000 * 2^LUX_SCALE
#define TSL2561_LUX_M8T (0x0000) ///< 0.000 * 2^LUX_SCALE

// Clipping thresholds (SI)
#define TSL2561_CLIPPING_13MS 4900 ///< # Counts that trigger a change in gain/integration

uint32_t calculateLux(uint16_t broadband, uint16_t ir);

#endif 

