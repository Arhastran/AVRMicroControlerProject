/*
 Adam Ignaciuk 
 Projek PSM 2021
 Dioda RC5, odbieranie sygnału IR i wyświetlanie wartości na ekranie 
 Komendy są zgodne z standardem Phillips http://www.easy-soft.net.pl/artykuly/jezyk-c/dekoder-kodu-rc5
 Podłączenia - LCD do PORTB, TSOP środek do PD4 zaś dioda do PA0
 */
#include <avr/io.h>              
#include <avr/interrupt.h>
#include <avr/delay.h>



volatile uint8_t RC5cmd;
volatile uint8_t RC5addr;
volatile uint8_t z;
volatile unsigned char ready;
volatile unsigned long int tmp;

SIGNAL(INT1_vect) //wektor przerwań 
{
   uint16_t ilosc_bitow = 0;
   uint8_t liczba_zboczy = 0;
   unsigned char mem_bit;
   uint8_t i = 1; // Jezeli nie ma bledu i = 1, jezeli jest => i = 0;
   
   GICR &= ~_BV(INT1); //zamaskowanie przerwania INT1
   TCCR0 = (1<<WGM01); //tryb CTC
   TCCR0 = (1<<CS02); //preskaler 256
   TCNT0 = 0; OCR0 = 81;// T=1,296ms ==> f=771,605Hz

   tmp = 0;

   while(i)
   {
      mem_bit=(!(PIND & 0x08));
      liczba_zboczy = 0;
      
      TIFR |= (1<<OCF0); //kasowanie flagi przerwania przez wpisanie jedynki      

      while(TIFR&OCF0)  //!OCF0
      {
         if(mem_bit != ((!(PIND & 0x08))))
         {
            liczba_zboczy++;
            mem_bit=(!(PIND & 0x08));

            if(liczba_zboczy>1) //wiecej niz jedno zbocze
            {
               //W przypadku błędu 
               i=0;
               GICR |= (1<<INT1);  //odblokowanie przerwania INT1
               return;
            }
         }
      }
      tmp<<=1;// przesuniecie wszystkich bitow o jeden w lewo
      tmp |= mem_bit; 
      TIFR |= (1<<OCF0); //kasowanie flagi przerwania przez wpisanie jedynki
      
      ilosc_bitow++;
   
      if(ilosc_bitow == 13)
      {
         RC5cmd = tmp&0x3f; //przyjmuje wartości w zależności od zebranych bitów w postaci liczby 0-63
         z = tmp; // przyjmuje postać w zależności od bitów jako 0-255
         RC5addr = (tmp>>6) & 0x1f;
         ready = 1;
         GICR |= (1<<INT1);  //odblokowanie przerwania INT1   
         i=0;
         return;   
      } 
     TCNT0 = 0; OCR0 = 81;// T=1,296ms ==> f=771,605Hz

   }
}

int main(void)
{
   DDRA &= ~(1<<0);
   DDRD = 0x00; //wejscia
   GICR |= (1<<INT1);  //odblokowanie przerwania INT1   
   MCUCR |= ((1<<ISC11)&(1<<ISC00)&(1<<ISC01)&(1<<ISC10)); // Przerwanie przy dowolnym zboczu 
   uint8_t volume = 0;
   uint8_t numerator = 0;
   lcdinit();
   lcd_clr();
   sei();
   
   lcd_text("Send IR signal");
  
   while(1)
   {
      if(ready)
      {
		  lcd_clr();
		  if (RC5cmd==16) // zwiększenie natężęnia dźwięku (vol up)
		  {
			  lcd_text("Signal detected");
			  goto_xy(0,1);
			  lcd_int(RC5cmd);
			  if (volume<101) volume+=1;
			  else volume =100;
			  lcd_text(" ");
			  lcd_int(volume);
		  }
		  else if (RC5cmd==17)// zmniejszanie natężęnia dźwięku (vol down)
		  {
			  lcd_text("Signal detected");
			  goto_xy(0,1);
			  lcd_int(RC5cmd);
			  if (volume>-1) volume-=1;
			  else volume =0;
			  lcd_text(" ");
			  lcd_int(volume);
		  }
		   else if (RC5cmd==53)// zapalanie diody (play)
		  {
			  lcd_text("Signal detected");
			  goto_xy(0,1);
			  if (numerator = 0) 
			  {
				PORTA &= ~(1<<PINA0);
				numerator = 1; 
			  }
			  else 
			  {
				PORTA &=(1<<PINA0); 
				numerator = 0;  
			  }
			  lcd_int(RC5cmd);
		  }
		   else 
		  {
			  lcd_text("Signal detected");
			  goto_xy(0,1);
			  lcd_int(RC5cmd);
		  }
	      _delay_ms(1000);
          ready = 0;
      }
   }
}
