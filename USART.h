/* 
 *  USART.h - Header file of PIC MPLABX USART library
 *  Written by Ercan Ersoy.
 *  Modified by Javier A. Pérez M.
 */

#ifndef USART_H
#define USART_H

#define _XTAL_FREQ 1000000

void USART_set(const unsigned long int baudrate);
void USART_send(const char data);
void USART_print(const char *string);

#endif
