/* 
 * File:   
 * Author: Javier Alejandro Pérez Marín
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef OSC_H
#define	OSC_H

#include <xc.h>
#include <stdint.h>

void int_osc_MHz(uint8_t freq);
/*
freq: 8 -> IRCF = 111 ->  FOSC = 8 MHz
freq: 4 -> IRCF = 110 ->  FOSC = 4 MHz
freq: 2 -> IRCF = 101 -> FOSC = 2 MHz
freq: 1 -> IRCF = 100 -> FOSC = 1 MHz
 */
#endif

