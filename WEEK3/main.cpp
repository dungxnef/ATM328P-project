#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>



#define KEY_PRT     PORTB
#define KEY_DDR     DDRB
#define KEY_PIN     PINB

#define LCD_Data_Dir DDRC
#define LCD_Command_Dir DDRD
#define LCD_Data_Port PORTC
#define LCD_Command_Port PORTD
#define RS PIND0
#define RW PIND1
#define EN PIND2

int keypad[4][4] = {
	{1, 2, 3, 4 },
	{5, 6, 7, 8 },
	{9, 10, 11, 12 },
	{13, 14, 15, 16}
};

volatile uint8_t keyReleased = 1; // Flag to track key release

void LCD_Command(unsigned char cmnd) {
	LCD_Data_Port = cmnd;
	LCD_Command_Port &= ~(1 << RS);
	LCD_Command_Port &= ~(1 << RW);
	LCD_Command_Port |= (1 << EN);
	_delay_us(1);
	LCD_Command_Port &= ~(1 << EN);
	_delay_ms(3);
}

void LCD_Char(unsigned char char_data) {
	LCD_Data_Port = char_data;
	LCD_Command_Port |= (1 << RS);
	LCD_Command_Port &= ~(1 << RW);
	LCD_Command_Port |= (1 << EN);
	_delay_us(1);
	LCD_Command_Port &= ~(1 << EN);
	_delay_ms(1);
}

void LCD_Init(void) {
	LCD_Command_Dir = 0xFF;
	LCD_Data_Dir = 0xFF;
	_delay_ms(20);
	
	LCD_Command(0x38);
	LCD_Command(0x0C);
	LCD_Command(0x06);
	LCD_Command(0x01);
	LCD_Command(0x80);
}

void LCD_String(char *str) {
	int i;
	for (i = 0; str[i] != 0; i++) {
		LCD_Char(str[i]);
	}
}

void LCD_String_xy(char row, char pos, char *str) {
	if (row == 0 && pos < 16)
	LCD_Command((pos & 0x0F) | 0x80);
	else if (row == 1 && pos < 16)
	LCD_Command((pos & 0x0F) | 0xC0);
	LCD_String(str);
}

void LCD_Clear() {
	LCD_Command(0x01);
	LCD_Command(0x80);
}

int main(void) {
	uint8_t r, c;
	KEY_DDR = 0x0F; // Set lower part of keypad port as output
	KEY_PRT = 0XFF; // Enable pull-ups for upper part (input)

	LCD_Init();
	LCD_String(" Press a key");

	while (1) {
		for (c = 0; c < 4; c++) {
			// Ground one column at a time
			KEY_PRT = (KEY_PRT & 0xF0) | (0x01 << c);
			
			for (r = 0; r < 4; r++) {
				if (!(KEY_PIN & (1 << (r + 4))) && keyReleased) {
					keyReleased = 0; // Key is pressed, set flag
					char str[2];
					sprintf(str,"%d", keypad[r][c]);
					LCD_Clear();
					LCD_String_xy(0, 0, " Key Pressed:");
					LCD_String_xy(1, 0, str);
					_delay_ms(200);
					keyReleased = 1; // Key is released, reset flag
				}
				else if (KEY_PIN & (1 << (r + 4))) {
					keyReleased = 1; // Key is released, reset flag
				}
			}
		}
	}

	return 0;
}
