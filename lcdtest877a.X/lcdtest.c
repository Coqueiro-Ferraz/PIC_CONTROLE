/*
 * File:   lcdtest.c
 * Author: Henrique
 *
 * Created on 21 de Abril de 2023, 10:43
 */

#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)

#include <xc.h>
#include "string.h"
#include "biblioteca_lcd.h"

#define _XTAL_FREQ 4000000

#include <string.h>

#define BUFFER_SIZE 50

#define BOTAO RC0

char buffer[BUFFER_SIZE]; // Buffer para armazenar a string recebida
int buffer_pos = 0; // Posi��o atual no buffer

char robo[8] = {0x0E, 0x11, 0x0E, 0x04,
                0x1F, 0x04, 0x0A, 0X10};

lcd_pin_t lcd_pins = {
    &PORTB, 0,
    &PORTB, 1,
    &PORTB, 2, 3, 4, 5
};

void uart_init() { // Inicializa a UART
    TRISCbits.TRISC7 = 1; // Define o pino RC7 como entrada (RX)
    TRISCbits.TRISC6 = 0; // Define o pino RC6 como sa�da (TX)

    TXSTA = 0b00100100; // Configura a transmiss�o: 8 bits, sem paridade, 1 stop bit, transmiss�o ass�ncrona
    RCSTA = 0b10010000; // Configura a recep��o: 8 bits, sem paridade, 1 stop bit, transmiss�o ass�ncrona
    //BAUDCON = 0b00001000; // Configura a baud rate: divis�o por 16 desativada
    
    SPBRG = 25; // Configura a baud rate para 9600 bps (Fosc = 8 MHz)

    RCIF = 0; // Limpa a flag de recep��o
    RCIE = 1; // Habilita a interrup��o de recep��o
    PEIE = 1; // Habilita as interrup��es perif�ricas
    GIE = 1; // Habilita as interrup��es globais
}

void main() 
{
    int ativo = 1;
    char apresenta[16];
    TRISB = 0x00;
    PORTB = 0x00;
    TRISC = 0xBF;
    PORTC = 0x00;
    
    uart_init();
    
    lcd_init(&lcd_pins);
    //escreve(2,3,"Henrique");   
    
    while(1)
    {
        if (BOTAO) { // Verifica se o bot�o foi pressionado
            __delay_ms(50); // Espera um tempo para evitar debounce
            while (BOTAO);  // Verifica novamente se o bot�o foi pressionado
            while (TRMT == 0); // Espera at� que o registrador de transmiss�o esteja vazio
            if(ativo == 1){
                TXREG = '0'; // Envia o n�mero 1 pela UART 
                ativo = 0;
            } else {
                TXREG = '1'; // Envia o n�mero 1 pela UART   
                ativo = 1;
            }
                
            
        }
        
        for(int i = 0; i<13; i++) apresenta[i]=buffer[i];
        apresenta[13] = 0;
        escreve (1,0,&apresenta[0]);
        escreve (2,0,&buffer[14]);
    }
}

void __interrupt() isr() { // Rotina de interrup��o da UART
    if (PIR1bits.RCIF) { // Verifica se h� dados dispon�veis na UART
        char data = RCREG; // L� o byte recebido
        if (data == '\n' || data == '\r') { // Verifica se a string foi finalizada
            buffer[buffer_pos] = '\0'; // Adiciona o caractere nulo ao final da string
            buffer_pos = 0; // Reinicia a posi��o do buffer
        } else if (buffer_pos < BUFFER_SIZE - 1) { // Verifica se ainda h� espa�o no buffer
            buffer[buffer_pos] = data; // Armazena o byte recebido no buffer
            buffer_pos++; // Incrementa a posi��o do buffer
        } else {
            buffer_pos = 0; // se receber dado maior que o buffer, sobrescreve primeiras posi��es 
            buffer[buffer_pos] = data; // Armazena o byte recebido no buffer
        }
    }
}
