/*
 * File:   main_slave_p1.c
 * Author: Javier Alejandro P�rez Mar�n y Jorge Ricardo Cer�n Cheley
 * 
 * Programaci�n PIC esclavo, este maneja la comunicaci�n con los sensores,
 * actuadores y env�a estado de los sensores
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
#include "pwm.h"
#include "USART.h"
#include "adc.h"


/*
 * Constantes
 */
#define _XTAL_FREQ 1000000 //Osc a 1 MHz

#define IN_MIN 0                
#define IN_MAX 1023 
#define OUT_MIN 0
#define OUT_MAX 45

/*
 * Variables
 */
uint16_t switch_servo = 0, last_mov = 5, TEMP_POT = 0, OLD_TEMP = 150;
/*
 * Prototipos de Funci�n
 */
void setup(void); //Funci�n para configuraciones
void servo(unsigned short mov); //Funci�n para accionar motor Servo en base a movimiento
unsigned short map(uint16_t  val, uint8_t  in_min, uint16_t in_max, 
                   unsigned short out_min, unsigned short out_max);
void motor_dc(int temp);
/*
 * Interrupciones
 */
void __interrupt() slave(void){
    if(INTCONbits.RBIF){ // Interrucpci�n PORTB
        if(!PORTBbits.RB1){  // �Sensor de movimiento en RB1?
            switch_servo = 1; // Si hay movimiento -> Abrir       
        }
        else{
            switch_servo = 0; // Si ya no hay movimiento -> Cerrar
        }
        INTCONbits.RBIF = 0; // Flag Int. Portb -> Clear
    }
    if(PIR1bits.ADIF){                      
        if(ADCON0bits.CHS == 0){ // Interrupci�n AN0
            TEMP_POT = map(adc_read(), IN_MIN, IN_MAX, OUT_MIN, OUT_MAX);            
        }
        PIR1bits.ADIF = 0;  // Flag Int. ADC -> Clear
    }
    return;
}

/*
 * Loop principal
 */

void main(void) {
    setup();
    while(1){
        adc_ch_switch(1); //CH1 -> Inicializaci�n de conversi�n
        motor_dc(TEMP_POT);
        servo(switch_servo);
    }
    return;
}

/*
 * Funciones
 */

unsigned short map(uint16_t x, uint8_t x0, uint16_t x1, 
            unsigned short y0, unsigned short y1){
    return (unsigned short)(y0+((float)(y1-y0)/(x1-x0))*(x-x0));
}
void motor_dc(int temp){
    if(temp >= 23){
        PORTBbits.RB0 = 1; // Se activa ventilador
    }
    else{
        PORTBbits.RB0 = 0; // Se detiene ventilador
    }
    
    if (temp != OLD_TEMP){
        OLD_TEMP = temp;
        USART_send(temp+128);
    }
    return;
}
void servo(unsigned short mov){
    if (mov == 1){
        pwm_duty_cycle(94); // PWM -> 90�
    }
    else{
        pwm_duty_cycle(31); // PWM -> 0�
    }
    
    if (mov != last_mov){ //Se actualiza sensor solo si hay cambio
        last_mov = mov;
        USART_send((('M')<<1) + !mov); //Envio codificado de sensor de movimiento
    }
    
    return;
}

/*
 * Configuraciones
 */
void setup(void){
    int_osc_MHz(1); //OSC a 1 MHz        
    
    //I/O DIGITALES - Excepto RA0
    ANSEL = 0x01; //RA0 -> Input anal�gico
    ANSELH = 0;
    TRISAbits.TRISA0 = 1; //RA0 -> Input 

    TRISBbits.TRISB0 = 0; //RB0 -> Output
    PORTBbits.RB0 = 0;  //RB0 -> Clear
    TRISBbits.TRISB1 = 1; //RB1 -> Input   
    PORTBbits.RB1 = 0;  //RB1 -> Clear
    OPTION_REGbits.nRBPU = 0; //Enable all pull ups
    WPUBbits.WPUB = 0x02; //RB1 Pull-up -> Enable
    
    pwm_init(1); //CCP1 -> Init
    USART_set(9600); //Inicializaci�n com. UART a BR 9600
    adc_init(0,0,0); // -> FOSC/2 , V_DD, V_SS, justificado a la izquierda (comunista xdd)
    
    //Interrupciones
    INTCONbits.GIE = 1; //Int. Globales
    INTCONbits.PEIE = 1; //Int. de perif�ricos
    INTCONbits.RBIE = 1; // Int. PORTB -> Enable
    IOCBbits.IOCB1 = 1; // IOCB RB1 -> Enable
    INTCONbits.RBIF = 0; // Flag Int. PORTB -> Clear
    
    return;
}