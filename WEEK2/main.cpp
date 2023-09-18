#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define LED_Direction DDRB		/* define LED Direction */
#define LED_PORT PORTB			/* define LED port */

#define BUTTON_DDR DDRC
#define BUTTON_PORT PORTC
#define BUTTON_PIN PINC
#define BUTTON_PIN_NUMBER PINC0

#define LCD_Data_Dir DDRB		/* Define LCD data port direction */
#define LCD_Command_Dir DDRC		/* Define LCD command port direction register */
#define LCD_Data_Port PORTB		/* Define LCD data port */
#define LCD_Command_Port PORTC		/* Define LCD data port */
#define RS PC1				/* Define Register Select (data/command reg.)pin */
#define RW PC2				/* Define Read/Write signal pin */
#define EN PC3				/* Define Enable signal pin */

void LCD_Command(unsigned char cmnd)
{
	LCD_Data_Port= cmnd;
	LCD_Command_Port &= ~(1<<RS);	/* RS=0 command reg. */
	LCD_Command_Port &= ~(1<<RW);	/* RW=0 Write operation */
	LCD_Command_Port |= (1<<EN);	/* Enable pulse */
	_delay_us(1);
	LCD_Command_Port &= ~(1<<EN);
	_delay_ms(3);
}

void LCD_Char (unsigned char char_data)	/* LCD data write function */
{
	LCD_Data_Port= char_data;
	LCD_Command_Port |= (1<<RS);	/* RS=1 Data reg. */
	LCD_Command_Port &= ~(1<<RW);	/* RW=0 write operation */
	LCD_Command_Port |= (1<<EN);	/* Enable Pulse */
	_delay_us(1);
	LCD_Command_Port &= ~(1<<EN);
	_delay_ms(1);
}

void LCD_Init (void)			/* LCD Initialize function */
{
	LCD_Command_Dir = 0xFF;		/* Make LCD command port direction as o/p */
	LCD_Data_Dir = 0xFF;		/* Make LCD data port direction as o/p */
	_delay_ms(20);			/* LCD Power ON delay always >15ms */
	
	LCD_Command (0x38);		/* Initialization of 16X2 LCD in 8bit mode */
	LCD_Command (0x0C);		/* Display ON Cursor OFF */
	LCD_Command (0x06);		/* Auto Increment cursor */
	LCD_Command (0x01);		/* Clear display */
	LCD_Command (0x80);		/* Cursor at home position */
}

void LCD_String (char *str)		/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)		/* Send each char of string till the NULL */
	{
		LCD_Char (str[i]);
	}
}

void LCD_String_xy (char row, char pos, char *str)/* Send string to LCD with xy position */
{
	if (row == 0 && pos<16)
	LCD_Command((pos & 0x0F)|0x80);	/* Command of first row and required position<16 */
	else if (row == 1 && pos<16)
	LCD_Command((pos & 0x0F)|0xC0);	/* Command of first row and required position<16 */
	LCD_String(str);		/* Call LCD string function */
}

void LCD_Clear()
{
	LCD_Command (0x01);		/* clear display */
	LCD_Command (0x80);		/* cursor at home position */
}

int main(void)
{
	LED_Direction |= 0xff;		/* define LED port direction is output */
 	LED_PORT = 0xff;
	
	BUTTON_DDR &= ~(1 << BUTTON_PIN_NUMBER);
	BUTTON_PORT |= (1 << BUTTON_PIN_NUMBER);
	
	LCD_Init();
	LCD_String(" 0 to 9 counter");
	
	
	int counter = 0 ;
	
	char array[]={
		0b00000000,
		0b00000001,
		0b00000010,
		0b00000011,
		0b00000100,
		0b00000101,
		0b00000110,
		0b00000111,
		0b00001000,
		0b00001001,
		};
	/* write hex value for 7 SEG display from 0 to 9 */
	
	while(1)
	{
		
		if (!(BUTTON_PIN & (1 << BUTTON_PIN_NUMBER))) {
			_delay_ms(100);
			LED_PORT = array[counter];
			char str[2];
			sprintf(str, "%d", counter); // Convert the counter to a string
			LCD_Clear();
			LCD_String_xy(0, 0, str); // Display the counter on the LCD
			counter++;
			_delay_ms(100);
			
			if (counter > 9 ){
				counter = 0;
				
			}
			
			
			while (!(BUTTON_PIN & (1 << BUTTON_PIN_NUMBER)));
		
		
		}
	}
}