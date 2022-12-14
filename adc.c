/*
 * File:   adc.c
 * Author: Jorge Cer?n
 *
 * Created on 21 de julio de 2022, 09:27 PM
 */
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 1000000
#endif /*_XTAL_FREQ*/

#include <xc.h>
#include "adc.h"

void adc_init(uint8_t adc_cs, uint8_t vref_plus, uint8_t vref_minus){
    switch(adc_cs){
        case 0:                     // FOSC/2
            ADCON0bits.ADCS = 0b00;
            break;
        case 1:                     // FOSC/8
            ADCON0bits.ADCS = 0b01;
            break;
        case 2:                     // FOSC/32
            ADCON0bits.ADCS = 0b10;
            break;
        case 3:                     // F_RC
            ADCON0bits.ADCS = 0b11;
            break;
        default:                    // FOSC/8
            ADCON0bits.ADCS = 0b01;
            break;
    }
    switch(vref_plus){
        case 0:                     // V_DD
            ADCON1bits.VCFG0 = 0;
            break;
        case 1:                     // V_REF+ pin
            ADCON1bits.VCFG0 = 1;
            break;
        default:                    // V_DD
            ADCON1bits.VCFG0 = 0;
            break;
    } 
    switch(vref_minus){
        case 0:                     // V_SS
            ADCON1bits.VCFG1 = 0;
            break;
        case 1:                     // V_REF- pin
            ADCON1bits.VCFG1 = 1;
            break;
        default:                    // V_SS
            ADCON1bits.VCFG1 = 0;
            break;
    }
    ADCON1bits.ADFM = 1;            // Justificador a la derecha
    ADCON0bits.ADON = 1;            // Habilitar modulo ADC
    PIE1bits.ADIE = 1;              // Habilitar interrupciones de ADC
    PIR1bits.ADIF = 0;              // Limpiar la bandera de ADC
}
void adc_start(uint8_t channel){
    if(ADCON0bits.GO == 0){         // Verificar si hay conversi?n en proceso
        ADCON0bits.CHS = channel;   // Selecci?n de canal
        __delay_us(40);             // Delay
        ADCON0bits.GO = 1;          // Iniciar conversi?n
    }    
}
uint16_t adc_read(void){
    PIR1bits.ADIF = 0;              // Limpiar la bandera de ADC
    return ((ADRESH << 8) + ADRESL);// Regreso de ADC
}

void adc_ch_switch(uint8_t channels){
    switch(channels){
        case 1:
            if (ADCON0bits.GO == 0){            // Verificar si hay conversi?n en proceso
                ADCON0bits.CHS = 0;             // Selecci?n de canal 0
                __delay_us(40);                 // Delay
                ADCON0bits.GO = 1;              // Iniciar conversi?n
            }
            break;
        case 2:
            if (ADCON0bits.GO == 0){            // Verificar si hay conversi?n en proceso
                if (ADCON0bits.CHS == 0){       // Revisar si est? en canal 0
                    ADCON0bits.CHS = 1;         // Cambiar a canal 1
                    __delay_us(40);             // Delay
                }
                else if (ADCON0bits.CHS == 1){  // Revisar si est? en canal 1
                    ADCON0bits.CHS = 0;         // Cambiar a canal 0
                    __delay_us(40);             // Delay
                }
                __delay_us(40);                 // Delay
                ADCON0bits.GO = 1;              // Iniciar conversi?n
            }
            break;
        case 3:
            if (ADCON0bits.GO == 0){            // Verificar si hay conversi?n en proceso
                if (ADCON0bits.CHS == 0){       // Revisar si est? en canal 0
                    ADCON0bits.CHS = 1;         // Cambiar a canal 1
                    __delay_us(40);             // Delay
                }
                else if (ADCON0bits.CHS == 1){  // Revisar si est? en canal 1
                    ADCON0bits.CHS = 2;         // Cambiar a canal 2
                    __delay_us(40);             // Delay
                }
                else if (ADCON0bits.CHS == 2){  // Revisar si est? en canal 2
                    ADCON0bits.CHS = 0;         // Cambiar a canal 0
                    __delay_us(40);             // Delay
                }
                __delay_us(40);                 // Delay
                ADCON0bits.GO = 1;              // Iniciar conversi?n
            }
            break;
        default:
            break;
    }
}