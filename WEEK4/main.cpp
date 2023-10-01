#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

void uart_init_rx() {
	UBRRH = 0;
	UBRRL = 51;
	
	UCSRA = 0x00;
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
	UCSRB = (1 << RXEN);
}


char uart_receive_char() {
	while (!(UCSRA & (1 << RXC)));
	return UDR;
}

int main(void) {

	uart_init_rx();

	DDRA = 0xFF;

	char received_char;

	while (1) {
		received_char = uart_receive_char();
		if (received_char >= '0' && received_char <= '9') {
			PORTA = received_char - '0';
			} else {
			PORTA = 0;
		}
	}
}