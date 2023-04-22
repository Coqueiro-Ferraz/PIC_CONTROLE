/*
 * File:   controle.c
 * Author: HAGACEEF
 *
 * Created on 21 de Abril de 2023, 23:16
 * 
 * Neste programa
 * 
 * Timer 0 pisca um led no RC3
 * Lê ADC de sensor de temperatura LM35 e com base no valor
 * aumenta o PWM para controle de um ventilador
 * 
 * O valor da temperatura e do duty cycle são enviados pela UART
 * se vier 1 pela Uart o PWM liga, se vier 0, desliga
 *  
 */

#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)

#include <xc.h>
#include "string.h"
#include <stdio.h>
#include "biblioteca_lcd.h"

#define _XTAL_FREQ 4000000

// Define LM35 pin
#define LM35_PIN RA0

// Define PWM pin
#define PWM_PIN RC2

// Define LED pin
#define LED_PIN RC3

char pwm_enabled;

lcd_pin_t lcd_pins = {
    &PORTB, 2,
    &PORTB, 3,
    &PORTB, 4, 5, 6, 7
};
void PWM_init() 
{
    TRISC2 = 1;             //para pwm
    PR2 = 255;              //define período
    CCP1CON = 0b00001100;   //configura registrador (0x00 para desabilitar)
    CCPR1L = 100;           //define duty inicial
    TMR2IF = 0;             //limpa flag timer2
    T2CON = 0b00000010;     //configura pré escala para 16
    T2CON = 0b00000110;     //liga timer2
    while(!TMR2IF);         //espera o primeiro estouro
    TRISC2 = 0;             //habilita saída
    TMR2IF = 0;             //limpa flag
    pwm_enabled = 1;
}

float LM35() 
{
    unsigned int adc_val;
    float temperatura;
    ADON = 1;            // Habilita
    ADFM = 0;            // Left
    ADCS1 = 0;           // clock Fosc/8
    ADCS0 = 1; 
    CHS3 = 0;   
    CHS2 = 0;  
    CHS1 = 0;  
    CHS0 = 0;      
    __delay_us(20);                
    GO_DONE = 1;         
    while (GO_DONE);     
    adc_val = ADRESH;             
    adc_val = (adc_val << 8) | ADRESL;
    temperatura = adc_val * 5.0f / 1023 * 1.543319f ; 
    ADON = 0;           
    return temperatura;
}

void Timer0_init() 
{
    PSA = 0;         // Uso do prescaler
    PS2 = 0b1;       // Prescaler em 1:64
    PS1 = 0b0;
    PS0 = 0b1;
    T0CS = 0;        // Use Timer0 internal clock
    T0IE = 1;        // Habilita interrupção do timer0
    T0IF = 0;        // Limpa flag
    TMR0 = 100;      // Carrega valor timer0
    GIE = 1;         // liga interrupção global
}

void UART_init() {
    // baud rate 9600
    SPBRG = 25;
    // Habilita transmit e receive
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;
    // Modo sícrono, dado 8-bit, sem paridade, 1 stop bit
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    TXSTAbits.TX9 = 0;
}

void temperatura()
{
    
}

void velocidade(char duty)
{
    CCPR1L = duty;
    TMR2 = 0;
}

void apresenta()
{
    
}

void tempo()
{
    
}

void controle_pwm(char ligado)
{
    if(ligado)
    {
        CCP1CON = 0b00001100; 
        TMR2 = 0;
        TRISC2 = 0;
        pwm_enabled = 1;
    }
    else
    {
        TRISC2 = 1;
        CCP1CON = 0b00000000; 
        pwm_enabled = 0;
    }
}

void main() 
{
    float temp;
    unsigned char duty_cycle;
    char buffer[30];
    ANSELH = 0;
    TRISB = 0;
    
    UART_init();
    PWM_init();
    Timer0_init();
    TRISCbits.TRISC3 = 0;
    lcd_init(&lcd_pins);
    escreve(1,1,"Controle");
    while (1) {
        
        temp = LM35();

        if (temp < 25) {
            duty_cycle = 0;
        } else if (temp > 50) {
            duty_cycle = 255;
        } else {
            duty_cycle = (unsigned char)((temp - 25) * 10.2);
        }
        //a função a seguir formata a string de envio para serial
        // ela consome muita memória
        sprintf(buffer, "ADC0: %.2f, Duty: %uc \r\n", temp, duty_cycle);
        for (int i = 0; buffer[i] != '\0'; i++) 
        {
            TXREG = buffer[i];
            while (!TXSTAbits.TRMT);
        }
        if (RCIF) 
        {
            char c = RCREG;
            if (c == '0' && pwm_enabled) {
                controle_pwm(0);
                TXREG = 'P'; // Envia o caracter 'P' pela UART
            } else if (c == '1' && !pwm_enabled) {
                controle_pwm(1);
                TXREG = 'P'; // Envia o caracter 'P' pela UART
            }
        }
        velocidade(duty_cycle);

        __delay_ms(100);
    }
}

void __interrupt() Timers_ISR() 
{
    if (INTCONbits.T0IF)            // Checa flag Timer0
    {
        INTCONbits.T0IF = 0;        // Limpa flag
        TMR0 = 100;                 // Carrega Timer0 (10ms)
        LED_PIN = !LED_PIN;         // inverte LED
    }
    if (TMR2IF)            // Checa flag Timer2
    {
        TMR2IF = 0;        // Limpa flag
    }
    
}