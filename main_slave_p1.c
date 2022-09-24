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
#include <math.h>
#include "osc.h"
#include "pwm.h"
#include "USART.h"
#include "adc.h"
#include "I2C.h"
#include "tmr0.h"
#include "tsl2561.h"

/*
 * Constantes
 */
#define _XTAL_FREQ 1000000 //Osc a 1 MHz


/*
 * Variables
 */
uint8_t p = 0, min = 0, hrs = 0, last_min = 61, last_hrs = 30, pulso = 1, dir = 5,
        lowCH0 = 0, highCH0 = 0, lowCH1 = 0, highCH1 = 0, LUX = 8, last_LUX = 15;
uint16_t switch_servo = 0, last_mov = 5, TEMP_POT = 0, OLD_TEMP = 150, CH0 = 0, CH1 = 0;
float R1 = 10000, logR2, R2, TC, A = 0.001129148, B = 0.000234125, C = 0.0000000876741;
/*
 * Prototipos de Función
 */
void setup(void); //Función para configuraciones
void servo(unsigned short mov); //Función para accionar motor Servo en base a movimiento
short map(uint16_t  val, uint16_t  in_min, uint16_t in_max, 
          short out_min, short out_max);
void motor_dc(int temp);
uint8_t bcd_dec(uint8_t no);
void RTC_read(void);
void pulse_step(unsigned short bt, unsigned short signal);
void LUX_config(void);
void LUX_read(void);
/*
 * Interrupciones
 */
void __interrupt() slave(void){
    if(INTCONbits.T0IF){
        pulso = !PORTDbits.RD1; //En cada interrupción se invierte pulso de step
        tmr0_reload(); //Reinicio TMR0
    }
    
    if(INTCONbits.RBIF){ // Interrucpción PORTB
        if(!PORTBbits.RB1){  // ¿Sensor de movimiento en RB1?
            switch_servo = 1; // Si hay movimiento -> Abrir       
        }
        else if(PORTBbits.RB1){
            switch_servo = 0; // Si ya no hay movimiento -> Cerrar
        }
        
        if(!PORTBbits.RB2){  // ¿Girar stepper a la derecha?
            dir = 1; // Se gira stepper a la derecha
            PORTDbits.RD2 = 1; //Si el botón está presionado -> Girar a la derecha
            
        }
                
        if(!PORTBbits.RB7){  // ¿Girar stepper a la izquierda?
            dir = 0; // Se gira stepper a la izquierda  
            PORTDbits.RD2 = 0; //Si el botón está presionado -> Girar a la izquierda
        }
        if(PORTBbits.RB2 && PORTBbits.RB7){
            dir = 5; // Sin acción
        }
        INTCONbits.RBIF = 0; // Flag Int. Portb -> Clear
    }
    
    if(PIR1bits.ADIF){                      
        if(ADCON0bits.CHS == 0){ // Interrupción AN0
            TEMP_POT = map(adc_read(), 0, 650, -55, 125);
            
            //Ecuación Stein-Hart psra conversión a temperatura de NTC
            R2 = R1 * ((float)(1023/TEMP_POT)-1);
            logR2 = log(R2);
            TC = (uint8_t)(273.15 - (1 / (A + B * logR2 + C * logR2 * logR2 * logR2)));
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
    LUX_config();
    while(1){
        PORTDbits.RD2 = dir & 0x01; //Constantemente se actualiza dirección de stepper
        pulse_step(dir, pulso); //Se acciona Stepper en base a la dirección del botón apachado
        adc_ch_switch(1); //CH1 -> Inicialización de conversión
        motor_dc(TC); //Se acciona motor DC en base a temperatura
        servo(switch_servo); //Se acciona servomotor en base a movimiento
        RTC_read(); //Se lee mediante I2C RTC
        LUX_read(); //Se lee mediante I2C TSL2561
    }
    return;
}

/*
 * Funciones
 */

short map(uint16_t x, uint16_t x0, uint16_t x1, 
          short y0, short y1){
    return (short)(y0+((float)(y1-y0)/(x1-x0))*(x-x0));
}
void motor_dc(int temp){
    if(temp >= 22){
        PORTBbits.RB0 = 1; // Se activa ventilador
    }
    else{
        PORTBbits.RB0 = 0; // Se detiene ventilador
    }

    if (temp != OLD_TEMP){
        OLD_TEMP = temp;
        USART_send(160 + (temp & 31)); //Envio codificado de sensor de temperatura (se limita hasta 31 °C)
    }
    return;
}
void servo(unsigned short mov){
    if (mov == 1){
        pwm_duty_cycle(94); // PWM -> 90°
    }
    else{
        pwm_duty_cycle(31); // PWM -> 0°
    }
    
    if (mov != last_mov){ //Se actualiza sensor solo si hay cambio
        last_mov = mov;
        USART_send(!mov + 128); //Envio codificado de sensor de movimiento
    }
    return;
}
void pulse_step(unsigned short bt, unsigned short signal){
    if (bt == 1){
        PORTDbits.RD1 = signal; //Si el botón acción está presionado -> Enviar señal 
    }
    else if(bt == 0){
        PORTDbits.RD1 = signal; //Si el botón acción está presionado -> Enviar señal 
    }
    else {PORTDbits.RD1 = 0;} //Se fuerza 0
    return;
}
void RTC_read(void){
    I2C_Master_Start(); //Start condition
    I2C_Master_Write(0xD0); // RTC ADDRESS
    I2C_Master_Write(0); // Se direcciona al registro 0
    I2C_Master_RepeatedStart(); //Reseteo I2C
    I2C_Master_Write(0xD1); // ADDRESS + READ BIT
    I2C_Master_Read(1); //(Read + Acknowledge) de segundos
    min = bcd_dec(I2C_Master_Read(1)); //(Read + Acknowledge) Minutos -> BCD a DEC
    hrs = bcd_dec(I2C_Master_Read(0)); //(Read + No Acknowledge) Horas -> BCD a DEC
    I2C_Master_Stop(); //Stop condition
    
    if (min != last_min){
        last_min = min;
        USART_send(min); //Envio de minutos
    }
    if (hrs != last_hrs){
        last_hrs = hrs;
        USART_send(hrs + 224); //Envio codificado de horas
    }
    if (min == 0 && p <= 8){
           p++;
           PORTDbits.RD6 = !PORTDbits.RD6; 
           __delay_ms(10);
    }
    else if (min != 0){
        PORTDbits.RD6 = 0; 
        p = 0;
    }
    else{
        PORTDbits.RD6 = 0; 
    }
    return;
}
uint8_t bcd_dec(uint8_t no){
    return ((no >> 4) * 10 + (no & 0x0F));
}
void LUX_config(void){
    //Configuración
    I2C_Master_Start(); //Start condition
    I2C_Master_Write(0x72); // TSL2561 ADDRESS (0x39) + WRITE
    I2C_Master_Write(0x80); // Se selecciona el registro de command y se direcciona al registro 0 (Control)
    I2C_Master_Write(0x03); //Power Up TSL2561
    I2C_Master_Stop(); //Stop condition
    
    I2C_Master_Start(); //Start condition
    I2C_Master_Write(0x72); // TSL2561 ADDRESS + WRITE
    I2C_Master_Write(0x81); // Se selecciona el registro de command y se direcciona al registro 1 (Timing Register)
    I2C_Master_Write(0x00); // Set -> Gain (1X) & Integration Time 13.7 ms
    I2C_Master_Stop(); //Stop condition    
    return;
}
void LUX_read(void){  
    //Read
    I2C_Master_Start(); //Start condition
    I2C_Master_Write(0x72); // TSL2561 ADDRESS (0x39) + WRITE
    I2C_Master_Write(0x8C); // Se selecciona el registro de command y se direcciona al registro 0xC (ADC CH0 low Data)
    I2C_Master_RepeatedStart(); //Reseteo I2C
    I2C_Master_Write(0x73); // TSL2561 ADDRESS (0x39) + READ (1)
    lowCH0 = I2C_Master_Read(1);
    highCH0 = I2C_Master_Read(1);
    lowCH1 = I2C_Master_Read(1);
    highCH1 = I2C_Master_Read(0);
    I2C_Master_Stop(); //Stop condition
    
    CH0 = (highCH0 << 8) + lowCH0; 
    CH1 = (highCH1 << 8) + lowCH1; 
    
    LUX = (uint8_t)( calculateLux(CH0, CH1) * 1/15);
    
    if (LUX != last_LUX){
        last_LUX = LUX;
        if (LUX >= 10)
            USART_send(192 + 10); //Envio codificado de nivel de luz
        else
            USART_send(192 + (LUX & 15)); //Envio codificado de nivel de luz
    }
    return;
}

/*
 * Configuraciones
 */
void setup(void){
    int_osc_MHz(1); //OSC a 1 MHz        
    
    //I/O DIGITALES - Excepto RA0
    ANSEL = 0x01; //RA0 -> Input analógico
    ANSELH = 0;
    
    TRISAbits.TRISA0 = 1; //RA0 -> Input 
    
    TRISAbits.TRISA4 = 0;
    PORTAbits.RA4 = 0;
    
    TRISBbits.TRISB0 = 0; //RB0 -> Output
    PORTBbits.RB0 = 0;  //RB0 -> Clear
    TRISBbits.TRISB1 = 1; //RB1 -> Input   
    PORTBbits.RB1 = 0;  //RB1 -> Clear
    TRISBbits.TRISB2 = 1; //RB2 -> Input
    PORTBbits.RB2 = 0;  //RB2 -> Clear
    TRISBbits.TRISB7 = 1; //RB7 -> Input
    PORTBbits.RB7 = 0;  //RB7 -> Clear
    OPTION_REGbits.nRBPU = 0; //Enable all pull ups
    WPUBbits.WPUB = 0x86; //RB1, RB2 y RB7 Pull-up -> Enable
    
    TRISDbits.TRISD1 = 0; //RD0 -> Output
    PORTDbits.RD1 = 0; // RD0 -> Clear
    TRISDbits.TRISD2 = 0; //RD1 -> Output
    PORTDbits.RD2 = 0; // RD2 -> Clear
    TRISDbits.TRISD6 = 0; //RD6 -> Output
    PORTDbits.RD6 = 0; // RD6 -> Clear
    
    pwm_init(1); //CCP1 -> Init
    USART_set(9600); //Inicialización com. UART a BR 9600
    adc_init(0,0,0); // -> FOSC/2 , V_DD, V_SS, justificado a la izquierda (comunista xdd)
    tmr0_init(16); //Inicialización TMR0 con PSA de 16
    tmr0_reload(); //TMR a 0.5 ms
    I2C_Master_Init(100000); //Se inicia I2C Master con 100KHz clock    
    
    //Interrupciones
    INTCONbits.GIE = 1; //Int. Globales
    INTCONbits.PEIE = 1; //Int. de periféricos
    INTCONbits.RBIE = 1; // Int. PORTB -> Enable
    IOCBbits.IOCB1 = 1; // IOCB RB1 -> Enable
    IOCBbits.IOCB2 = 1; // IOCB RB2 -> Enable
    IOCBbits.IOCB7 = 1; // IOCB RB7 -> Enable
    INTCONbits.RBIF = 0; // Flag Int. PORTB -> Clear
    
    return;
}