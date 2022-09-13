/*
 * File:   osc Mhz.c
 * Author: Javier Alejandro Pérez Marín
 * 
 * Función para selección de reloj interno del PIC
 *
 * Created on 22 de julio de 2022, 12:17 PM
 */

#include <stdint.h>
#include "osc.h"

void int_osc_MHz(uint8_t freq){
    switch(freq){
        case 1:
            OSCCONbits.IRCF = 0b100; // FOSC = 1 MHZ
            break;
        case 2:
            OSCCONbits.IRCF = 0b101; // FOSC = 2 MHz
            break;
        case 4:
            OSCCONbits.IRCF = 0b110; // FOSC = 4 MHz
            break;
        case 8:
            OSCCONbits.IRCF = 0b111; // FOSC = 8 MHz
            break;
        default: //Default en caso el usuario indique valor no definido
            OSCCONbits.IRCF = 0b110; // FOSC = 4 MHz
            break;
    }
    OSCCONbits.SCS = 1;
}

    