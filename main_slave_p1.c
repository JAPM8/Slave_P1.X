/*
 * File:   main_slave_p1.c
 * Author: Javier Alejandro Pérez Marín y Jorge Ricardo Cerón Cheley
 * 
 * Programación PIC esclavo, este maneja la comunicación con los sensores,
 * actuadores y envía estado de los sensores
 *
 * Created on 9 de septiembre de 2022, 02:18 PM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR21V   // Brown-out Reset Selection bit (Brown-out Reset set to 2.1V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/*
 * Librerias
 */
#include <xc.h>
#include <pic16f887.h>
#include <stdint.h>
#include "osc.h"
#include "USART.h"

/*
 * Constantes
 */
#define _XTAL_FREQ 1000000 //Osc a 1 MHz

/*
 * Variables
 */

/*
 * Prototipos de Función
 */
void setup(void); //Función para configuraciones

/*
 * Interrupciones
 */
/*void __interrupt() slave(void){
    
    return;
}*/

/*
 * Loop principal
 */

void main(void) {
    setup();
    while(1){
        USART_send('A');
        __delay_ms(100); 
    }
    return;
}

/*
 * Configuraciones
 */
void setup(void){
    int_osc_MHz(1); //OSC a 1 MHz
    
    //I/O DIGITALES
    ANSEL = 0;
    ANSELH = 0;
    
    TRISA = 0; //PORTA -> Output 
    PORTA = 0;
    
    USART_set(9600);
    
    //Interrupciones
    INTCONbits.GIE = 1; //Int. Globales
    INTCONbits.PEIE = 1; //Int. de periféricos
    return;
}