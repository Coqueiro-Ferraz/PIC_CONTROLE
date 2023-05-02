#ifndef _BIBLIOTECA_LCD_H_
#define _BIBLIOTECA_LCD_H_


#include <xc.h> 

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 4000000
#endif

typedef struct {
    volatile unsigned char *port_rs;
    unsigned char pino_rs;
    volatile unsigned char *port_en;
    unsigned char pino_en;
    volatile unsigned char *port_data;
    unsigned char pino_d4;
    unsigned char pino_d5;
    unsigned char pino_d6;
    unsigned char pino_d7;
} lcd_pin_t;

static void muda_pin(volatile unsigned char *_port, int _pin, int _val);
static void enable(void);
static void nibble(int dado);
static void byte (int escreve, char dado);
void lcd_init( lcd_pin_t *lcd_pins);
void limpa(void);
void escreve(char lin, char col, char *str);
void desloca(char casas, char esq);
void grava_caractere (char pos, char *linhas);
void usa_caractere (char lin, char col, char caractere);

#endif