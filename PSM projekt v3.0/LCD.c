#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#define LCD_PORT_DDR DDRB
#define LCD_PORT PORTB
// poniżej połączenia są definiowane. Kolejne wejścia LCD do pinów portu B
#define LCD_RS 0 //PB_0, więc pierwsze (RS) podłączamy do PB_0
#define LCD_E 1
#define LCD_D4 2
#define LCD_D5 3
#define LCD_D6 4
#define LCD_D7 5
#define SET_RS LCD_PORT |=(1<<LCD_RS)
#define SET_E LCD_PORT |=(1<<LCD_E)
#define CLR_RS LCD_PORT &=~(1<<LCD_RS)
#define CLR_E LCD_PORT &=~(1<<LCD_E)

//funkcja wysyłająca 4 bity 
static inline void lcd_send_half_byte(uint8_t data) //uzywana tylko w tym bloku i dla tej biblioteki (inline) 
{
	SET_E;
	if (data & (1<<0)) LCD_PORT |=(1<< LCD_D4); else LCD_PORT &=~(1 << LCD_D4);
	if (data & (1<<1)) LCD_PORT |=(1<< LCD_D5); else LCD_PORT &=~(1 << LCD_D5);
	if (data & (1<<2)) LCD_PORT |=(1<< LCD_D6); else LCD_PORT &=~(1 << LCD_D6);
	if (data & (1<<3)) LCD_PORT |=(1<< LCD_D7); else LCD_PORT &=~(1 << LCD_D7);
	CLR_E;
}
void lcd_write_byte(uint8_t data)
{
	lcd_send_half_byte(data >> 4); //przesłanie najstarszych bitów, najbardziej znaczących MSB
	_delay_us(120);
	lcd_send_half_byte(data); //przesłanie najmłodszych bitów, najmniej znaczących LSB
	_delay_us(120);
}

void lcd_write_cmd(uint8_t cmd)
{
	CLR_RS;
	lcd_write_byte(cmd);
}
void lcd_write_data(uint8_t data)
{
	SET_RS;
	lcd_write_byte(data);
}
void lcdinit(void)
{
	LCD_PORT_DDR |=(1<<LCD_RS); //ustawiamy kierunek czyli wyjscie
	LCD_PORT_DDR |=(1<<LCD_E);
	LCD_PORT_DDR |=(1<<LCD_D4)|(1<<LCD_D5)|(1<<LCD_D6)|(1<<LCD_D7);
	
	CLR_RS; //zerowanie RS, E, pinów na porcie 
	CLR_E; //ustawiamy zero bo nie wiemy jaki jest początkowy stan 
	LCD_PORT=0x00;
	/////////////
	_delay_ms(45);
	lcd_send_half_byte(3); //albo lcd_send_half_byte((1<<0)|(1<<1))
	_delay_ms(4.1);
	lcd_send_half_byte(3);
	_delay_us(100);
	lcd_send_half_byte(3);
	_delay_us(100);
	
	lcd_send_half_byte((1<<1));
	_delay_ms(3);
	
	lcd_write_cmd((1<<3)|(1<<5)); //Function set
	_delay_ms(3);
	
	lcd_write_cmd((1<<3)); //Display off
	_delay_ms(3);
	
	lcd_write_cmd((1<<0)); //Display clear
	_delay_ms(3);
	
	lcd_write_cmd((1<<2)|(1<<1)); //Entry mode set
	_delay_ms(3);
	
	lcd_write_cmd(0x0F);//lcd_write_cmd((1<<1)|(1<<2)|(1<<3)); // set display display on cursor on 
	_delay_ms(3);
}
void lcd_clr(void) //czyszczonko ekranu 
{
	lcd_write_cmd((1<<0));
	_delay_ms(5);
}
void lcd_cursor_return(void)
{
	lcd_write_cmd((1<<1));
	_delay_ms(5);
}
void lcd_text(char *str)
{
	while (*str) lcd_write_data(*str++);
}
void lcd_int(int16_t z) //wyswietla wartosc jako zmienna
{
	char buf[10];
	lcd_text(itoa(z,buf,10));
}
void goto_xy (uint8_t x, uint8_t y)
{
	uint8_t addr=0;
	if(y==0) addr=0x00+x;
	if(y==1) addr=0x40+x;
	lcd_write_cmd((1<<7) | addr);
}
void lcd_image(void)
{
	lcd_write_cmd((1<<6));
	lcd_write_data(0);
	lcd_write_data(20);
	lcd_write_data(27);
	lcd_write_data(30);
	lcd_write_data(27);
	lcd_write_data(27);
	lcd_write_data(20);
	lcd_write_data(0);
}



