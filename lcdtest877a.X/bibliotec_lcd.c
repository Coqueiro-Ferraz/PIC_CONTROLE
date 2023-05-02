#include "biblioteca_lcd.h"


volatile int RS, EN, D4, D5, D6, D7;
volatile unsigned char *port_rs,*port_en,*port_data;

static void muda_pin(volatile unsigned char *_port, int _pin, int _val)
{
    if(_val == 1)
    {
        *_port |= 1 << _pin;
    }
    else
    {
        *_port &= ~(1 << _pin);
    }
}


static void enable()
{
    muda_pin(port_en, EN, 1);
    __delay_ms(5);
    muda_pin(port_en, EN, 0);
    __delay_ms(1);
}

static void nibble(int dado)
{        
    muda_pin(port_data, D4, dado & 0x01);
    muda_pin(port_data, D5, dado >> 1 & 0x01);
    muda_pin(port_data, D6, dado >> 2 & 0x01);
    muda_pin(port_data, D7, dado >> 3 & 0x01);
  
}

static void byte (int escreve, char dado)
{
    muda_pin(port_rs, RS, escreve & 0x01);
    //RS = escreve & 0x01; //se RS = 0 é comando, se RS = 1 é escrita
    nibble(dado >> 4);
    enable();
    nibble(dado & 0x0F);
    enable();
}

void lcd_init( lcd_pin_t *lcd_pins)
{
    
    port_rs = lcd_pins->port_rs;
    port_en = lcd_pins->port_en;
    port_data = lcd_pins->port_data;
    RS = lcd_pins->pino_rs;
    EN = lcd_pins->pino_en;
    D4 = lcd_pins->pino_d4;
    D5 = lcd_pins->pino_d5;
    D6 = lcd_pins->pino_d6;
    D7 = lcd_pins->pino_d7;
    
    __delay_ms(50);
    nibble(0x2);
    enable();
    
    byte(0, 0x28);
    byte(0, 0x0C);
    byte(0, 0x06);
    byte(0, 0x01);  
    
    __delay_ms(5);
    
   // byte(1, 0x33);
    
}

void limpa()
{
    byte (0, 0x01);
    __delay_ms(5);
}

void escreve(char lin, char col, char *str)
{
    if(lin == 1)byte(0,0x80 + col);
    else byte(0,0xC0 + col);
    while (*str)
    {
        byte(1, *str++);       
    }
}

void desloca(char casas, char esq)
{
        for(int i = 0; i < casas; i++)
        {
            byte(0, (esq == 1 ? 0x1C : 0x18));
            __delay_ms(50);
        }
}


//cgram 0b01eeeeee
void grava_caractere (char pos, char *linhas)
{
    byte(0, 0x40);
    for (int i=0; i<8; i++)
    {
        byte(1, linhas[i]);
    }        
}

void usa_caractere (char lin, char col, char caractere)
{
    if(lin == 1)byte(0,0x80 + col);
    else byte(0,0xC0 + col);
    byte(1, caractere);     
}
