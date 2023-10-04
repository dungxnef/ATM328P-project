#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

// Define keypad and LCD connections
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

// Keypad layout
unsigned char keypad[4][4] = {
{'7', '8', '9', '+'},
{'4', '5', '6', '-'},
{'1', '2', '3', '*'},
{'C', '0', '=', '/'}
};

char input_buffer[17]; //  store keypad input (up to 16 characters)
char result_str[17]; // store result
int result;

unsigned char colloc, rowloc;

char keyfind(){
while (1)
{
	KEY_DDR = 0xF0; /* set port direction as input-output */
	KEY_PRT = 0x0F;
	do
	{
		KEY_PRT &= 0x0F; /* mask PORT for column read only */
		asm("NOP");
		colloc = (KEY_PIN & 0x0F); /* read status of column */
	} while (colloc != 0x0F);

	do
	{
		do
		{
			_delay_ms(20);             /* 20ms key debounce time */
			colloc = (KEY_PIN & 0x0F); /* read status of column */
			} while (colloc == 0x0F);        /* check for any key press */

			_delay_ms(20);              /* 20 ms key debounce time */
			colloc = (KEY_PIN & 0x0F);
		} while (colloc == 0x0F);

		/* now check for rows */
		KEY_PRT = 0xEF;             /* check for pressed key in 1st row */
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if (colloc != 0x0F)
		{
			rowloc = 0;
			break;
		}

		KEY_PRT = 0xDF;             /* check for pressed key in 2nd row */
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if (colloc != 0x0F)
		{
			rowloc = 1;
			break;
		}

		KEY_PRT = 0xBF;             /* check for pressed key in 3rd row */
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if (colloc != 0x0F)
		{
			rowloc = 2;
			break;
		}

		KEY_PRT = 0x7F;             /* check for pressed key in 4th row */
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if (colloc != 0x0F)
		{
			rowloc = 3;
			break;
		}
	}

	if (colloc == 0x0E)
	return (keypad[rowloc][0]);
	else if (colloc == 0x0D)
	return (keypad[rowloc][1]);
	else if (colloc == 0x0B)
	return (keypad[rowloc][2]);
	else
	return (keypad[rowloc][3]);
}


void LCD_Command(unsigned char cmnd)
{
	LCD_Data_Port = cmnd;
	LCD_Command_Port &= ~(1 << RS);
	LCD_Command_Port &= ~(1 << RW);
	LCD_Command_Port |= (1 << EN);
	_delay_us(1);
	LCD_Command_Port &= ~(1 << EN);
	_delay_ms(3);
}

void LCD_Char(unsigned char char_data)
{
	LCD_Data_Port = char_data;
	LCD_Command_Port |= (1 << RS);
	LCD_Command_Port &= ~(1 << RW);
	LCD_Command_Port |= (1 << EN);
	_delay_us(1);
	LCD_Command_Port &= ~(1 << EN);
	_delay_ms(1);
}

void LCD_Init(void)
{
	LCD_Command_Dir = 0xFF;
	LCD_Data_Dir = 0xFF;
	_delay_ms(20);

	LCD_Command(0x38);
	LCD_Command(0x0C);
	LCD_Command(0x06);
	LCD_Command(0x01);
	LCD_Command(0x80);
}

void LCD_String(char *str)
{
	int i;
	for (i = 0; str[i] != 0; i++)
	{
		LCD_Char(str[i]);
	}
}

void LCD_String_xy(char row, char pos, char *str)
{
	if (row == 0 && pos < 16)
	LCD_Command((pos & 0x0F) | 0x80);
	else if (row == 1 && pos < 16)
	LCD_Command((pos & 0x0F) | 0xC0);
	LCD_String(str);
}

void LCD_Clear()
{
	LCD_Command(0x01);
	LCD_Command(0x80);
}

void Oflow_err_msg(){
LCD_Clear();
LCD_String("Overflow Error!");
input_buffer[0] = '\0'; // Reset the input buffer
}

void Uflow_error_msg(){
LCD_Clear();
LCD_String("Underflow Error!");
input_buffer[0] = '\0'; // Reset the input buffer
}


int eval_expression(const char *expression, int *result)
{
int num = 0;
int temp_result = 0;
char op = '+';
int sign = 1;
int stack[100], top = -1;

for (int i = 0; expression[i] != '\0'; i++)
{
	char current_char = expression[i];
	if (isdigit(current_char))
	{
		int digit = (current_char - '0') * sign;
		if ((sign == 1 && num > (INT_MAX - digit) / 10) || (sign == -1 && num < (INT_MIN - digit) / 10)) {
			return 0;
		}
		num = num * 10 + digit;
	}

	else
	{
		switch (op)
		{
			case '+':
			if (num > 0 && stack[top] > INT_MAX - num) {
				Oflow_err_msg();
			}
			else if (num < 0 && stack[top] < INT_MIN - num) {
				Uflow_error_msg();
			}
			stack[++top] = num;
			break;
			case '-':
			if (num < 0 && stack[top] > INT_MAX + num) {
				Oflow_err_msg();
			}
			else if (num > 0 && stack[top] < INT_MIN + num) {
				Uflow_error_msg();
			}
			stack[++top] = -num;
			break;
			case '*':
			if ((stack[top] > 0 && num > INT_MAX / stack[top]) || (stack[top] < 0 && num < INT_MAX / stack[top])) {
				Oflow_err_msg();
			}
			else if ((stack[top] > 0 && num < INT_MIN / stack[top]) || (stack[top] < 0 && num > INT_MIN / stack[top])) {
				Uflow_error_msg();
			}
			stack[top] *= num;
			break;
			case '/':
			// Check for division by zero
			if (num == 0) {
				return 0; // Division by zero error
			}
			stack[top] /= num;
			break;
			default:
			return 0; // Invalid operator
		}

		num = 0; // Reset the number
		op = current_char; // Set the current operator

		if (expression[i + 1] == '-')
		{
			sign = -1;
			i++;
		}
	}
}

switch (op)
{
	case '+':
	if (num > 0 && stack[top] > INT_MAX - num) {
		Oflow_err_msg();
		return -1;
	}
	else if (num < 0 && stack[top] < INT_MIN - num) {
		Uflow_error_msg();
		return -2;
	}
	stack[++top] = num;
	break;
	case '-':
	if (num < 0 && stack[top] > INT_MAX + num) {
		Oflow_err_msg();
		return -1;
	}
	else if (num > 0 && stack[top] < INT_MIN + num) {
		Uflow_error_msg();
		return -2;
	}
	stack[++top] = -num;
	break;
	case '*':
	if ((stack[top] > 0 && num > INT_MAX / stack[top]) || (stack[top] < 0 && num < INT_MAX / stack[top])) {
		Oflow_err_msg();
		return -1;
	}
	else if ((stack[top] > 0 && num < INT_MIN / stack[top]) || (stack[top] < 0 && num > INT_MIN / stack[top])) {
		Uflow_error_msg();
		return -2;
	}
	stack[top] *= num;
	break;
	case '/':
	// Check for division by zero
	if (num == 0) {
		return 0; // Division by zero error
	}
	stack[top] /= num;
	break;
	default:
	return 0; // Invalid operator
}

while (top != -1)
{
	temp_result += stack[top--];
}

	*result = temp_result;

return 1; // Successful evaluation
}


int main(void) {

	LCD_Init();
	LCD_String(" 16 bit simple");
	LCD_String_xy(1,3,"Calculator");
	while(1)
	{
		char key = keyfind();
		
		if (key != ' ')
		{
			if (isdigit(key))
			{
				// Append the pressed number to the input buffer
				int input_length = strlen(input_buffer);
				if (input_length < 16)
				{
					input_buffer[input_length] = key;
					input_buffer[input_length + 1] = '\0'; // Null-terminate the string
					LCD_Clear();
					LCD_String_xy(0, 0, input_buffer);
				}
				else
				{
					// Handle buffer overflow error
					LCD_Clear();
					LCD_String("Buffer Overflow!");
				}
			}
			else if (key == '+' || key == '-' || key == '*' || key == '/')
			{
				// Display the operator on the LCD
				int input_length = strlen(input_buffer);
				if (input_length < 16)
				{
					input_buffer[input_length] = key;
					input_buffer[input_length + 1] = '\0'; // Null-terminate the string
					LCD_Clear();
					LCD_String_xy(0, 0, input_buffer);
				}
				else
				{
					// Handle buffer overflow error
					LCD_Clear();
					LCD_String("Buffer Overflow!");
				}
			}
			else if (key == '=')
			{
				// Check if the input buffer contains a valid expression
				if (strlen(input_buffer) > 0)
				{
					switch(eval_expression(input_buffer,&result))
					{
						case -2:
						Uflow_error_msg();
						break;
						case -1:
						Oflow_err_msg();
						break;
						case 0:
						LCD_Clear();
						LCD_String("Invalid Input");
						input_buffer[0] = '\0'; // Reset the input buffer
						break;
						default:
						sprintf(result_str, "%d", result);
						LCD_String_xy(1, 0, result_str);
						input_buffer[0] = '\0'; // Reset the input buffer
							
					}
					
				}
			}
			else if (key == 'C') // Clear the input buffer
			{
				input_buffer[0] = '\0';
				LCD_Clear();
			}
		}
	}

	return 0;
}
