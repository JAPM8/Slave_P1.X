/*
 * File: pwm.c
 * Author: Jorge Cerón
 *
 * Created on 22 de julio de 2022, 11:37 AM
 */

#include <xc.h>
#include "pwm.h"
#include <stdint.h>

void pwm_init (uint8_t channel) {
    switch(channel) {
        case 1:                         // CCP1
            TRISCbits.TRISC2 = 1;       // Deshabilitar salida de CCP1 
            CCP1CON = 0;                // Apagar CCP1
            CCP1CONbits.P1M = 0;        // Modo sigle output
            CCP1CONbits.CCP1M = 0b1100; // PWM
            break;
        case 2:                         // CCP2
            TRISCbits.TRISC1 = 1;       // Deshabilitar salida de CCP2
            CCP2CON = 0;                // Apagar CCP2
            CCP2CONbits.CCP2M = 0b1100; // PWM
            break;
        default:
            break;
    }
    T2CONbits.T2CKPS = 0b11;            // Prescaler = 16
    PR2 = 255;                          // 16 ms  
    // Periodo PWM = (PR2+1)4(1/Fosc)*(Valor del Prescaler TMR2)
    // Periodo PWM = (255+1)4(1/(1*10^6))*(16) = 16 ms
    PIR1bits.TMR2IF = 0;                // Limpiar bandera de TMR2
    T2CONbits.TMR2ON = 1;               // Encender TMR2
    while(!PIR1bits.TMR2IF);            // Esperar un ciclo del TMR2
    PIR1bits.TMR2IF = 0;                // Limpiar bandera de TMR2
    TRISCbits.TRISC2 = 0;               // Habilitar salida de PWM
    //TRISCbits.TRISC1 = 0;               // Habilitar salida de PWM     
}

void pwm_duty_cycle (uint16_t duty_cycle){
    CCPR1L = (uint8_t)(duty_cycle>>2); 
    CCP1CONbits.DC1B = duty_cycle & 0b11;
    //CCPR2L = (uint8_t)(duty_cycle>>2);
    //CCP2CONbits.DC2B0 = duty_cycle & 0b01; 
    //CCP2CONbits.DC2B1 = (duty_cycle & 0b10)>>1 ;
}