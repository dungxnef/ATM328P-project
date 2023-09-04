

#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz clockspeed
#endif

#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    DDRB = 0xFF; // Makes PORTB as Output
    while (1) 
    {
		PORTB = 0xFF; // Turn on LED
		_delay_ms(1000); // delay
		PORTB  = 0x00; // Turn off LED
		_delay_ms(1000);
    }
}

