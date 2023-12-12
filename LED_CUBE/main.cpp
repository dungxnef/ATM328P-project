#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define COLUMNS_DDRD DDRD
#define COLUMNS_DDRB DDRB
#define COLUMNS_DDRC DDRC

// blink each led vertical
void blink(uint8_t layer, uint8_t column) {
	PORTD = 0xFF;
	PORTB = 0xFF;
	PORTC = 0xFF;

	// Preserve the state of PC4 and PC5
	PORTC |= 0x30;

	// Activate the selected layer
	PORTC = (PORTC & 0xF0) | (1 << layer);

	// Specify the order in which the columns should be activated
	uint8_t column_order[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

	// Activate the selected column
	if (column_order[column] < 8) {
		PORTD &= ~(1 << column_order[column]);
		} else if (column_order[column] < 14) {
		PORTB &= ~(1 << (column_order[column] - 8));
		} else if (column_order[column] == 14) {
		PORTC &= ~(1 << PINC4);
		} else if (column_order[column] == 15) {
		PORTC &= ~(1 << PINC5);
	}
}
void reverse_blink(uint8_t layer) {
	for (int8_t j = 15; j >=0 ; j--) {
		blink(layer, j);
		_delay_ms(10);
	}
}
// blink each led horizontal
void blink_hori(uint8_t layer, uint8_t column) {
	PORTD = 0xFF;
	PORTB = 0xFF;
	PORTC = 0xFF;

	// Preserve the state of PC4 and PC5
	PORTC |= 0x30;

	// Activate the selected layer
	PORTC = (PORTC & 0xF0) | (1 << layer);

	// Specify the order in which the columns should be activated
	uint8_t column_order[] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};

	// Activate the selected column
	if (column_order[column] < 8) {
		PORTD &= ~(1 << column_order[column]);
		} else if (column_order[column] < 14) {
		PORTB &= ~(1 << (column_order[column] - 8));
		} else if (column_order[column] == 14) {
		PORTC &= ~(1 << PINC4);
		} else if (column_order[column] == 15) {
		PORTC &= ~(1 << PINC5);
	}
}
void reverse_blink_hori(uint8_t layer) {
	for (int8_t j = 15; j >=0 ; j--) {
		blink_hori(layer, j);
		_delay_ms(10);
	}
}

//blink columns on every layers vertical
void blink_col (uint8_t column) {
	 // Set all columns to inactive state
	 PORTD = 0xFF;
	 PORTB = 0xFF;
	 PORTC = 0xFF;

	 // Preserve the state of PC4 and PC5
	 PORTC |= 0x30;

	 // Specify the order in which the columns should be activated
	 uint8_t column_order[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

	 // Activate the selected column
	 if (column_order[column] < 8) {
		 PORTD &= ~(1 << column_order[column]);
		 } else if (column_order[column] < 14) {
		 PORTB &= ~(1 << (column_order[column] - 8));
		 } else if (column_order[column] == 14) {
		 PORTC &= ~(1 << PINC4);
		 } else if (column_order[column] == 15) {
		 PORTC &= ~(1 << PINC5);
	 }
 }
void r_blink_col() {
	for (int8_t j = 15; j >=0 ; j--) {
		blink_col(j);
		_delay_ms(10);
	}
}
//blink columns on every layers horizontal
void blink_col_hori (uint8_t column) {
	// Set all columns to inactive state
	PORTD = 0xFF;
	PORTB = 0xFF;
	PORTC = 0xFF;

	// Preserve the state of PC4 and PC5
	PORTC |= 0x30;

	// Specify the order in which the columns should be activated
	uint8_t column_order[] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};

	// Activate the selected column
	if (column_order[column] < 8) {
		PORTD &= ~(1 << column_order[column]);
		} else if (column_order[column] < 14) {
		PORTB &= ~(1 << (column_order[column] - 8));
		} else if (column_order[column] == 14) {
		PORTC &= ~(1 << PINC4);
		} else if (column_order[column] == 15) {
		PORTC &= ~(1 << PINC5);
	}
}
void r_blink_col_hori() {
	for (int8_t j = 15; j >=0 ; j--) {
		blink_col_hori(j);
		_delay_ms(10);
	}
}

// light led through column
void light(uint8_t layer) {
	// Set all columns to inactive state
	PORTD = 0xFF;
	PORTB = 0xFF;
	PORTC = 0xFF;

	// Preserve the state of PC4 and PC5
	PORTC |= 0x30;

	// Activate the selected layer
	PORTC = (PORTC & 0xF0) | (1 << layer);

	// Specify the columns you want to light up
	uint8_t columns[] = {0, 1, 2, 4, 6, 7, 8, 11, 12, 13, 14, 15};
	uint8_t num_columns = sizeof(columns) / sizeof(columns[0]);

	// Loop through each column in the array
	for (uint8_t i = 0; i < num_columns; i++) {
		uint8_t column = columns[i];

		// Activate the selected column
		if (column < 8) {
			PORTD &= ~(1 << column);
			} else if (column < 14) {
			PORTB &= ~(1 << (column - 8));
			} else if (column == 14) {
			PORTC &= ~(1 << PINC4);
			} else if (column == 15) {
			PORTC &= ~(1 << PINC5);
		}
	}
}

void chase(uint8_t column) {
	for (uint8_t i = 0; i < 4; i++) {
		blink(i, column);
		_delay_ms(20);
	}
}

void chase_h(uint8_t column) {
	for (uint8_t i = 0; i < 4; i++) {
		blink_hori(i, column);
		_delay_ms(20);
	}
}

int main(void) {
	COLUMNS_DDRD = 0xFF; // Set all pins on PORTD to output
	COLUMNS_DDRB = 0x3F; // Set PB0 to PB5 on PORTB to output
	DDRC = 0x7F; // Set PC0 to PC5 as output

	while(1) {
		for(uint8_t layer = 0; layer < 4; layer++) {
			light(layer);
			_delay_ms(50); 
		}
		for(uint8_t i = 0; i < 4; i++) {
			for(int8_t j = 0; j < 16; j++) {
				blink_hori(i, j);
				_delay_ms(10);
			}
		}
		for (int8_t i = 4; i >=0 ; i--) {
			reverse_blink_hori(i);
			_delay_ms(10);
		}
		_delay_ms(100);
		for(uint8_t i = 0; i < 4; i++) {
			for(uint8_t j = 0; j < 16; j++) {
				blink(i, j);
				_delay_ms(10);
			}
		}
		for (int8_t i = 4; i >=0 ; i--) {
			reverse_blink(i);
			_delay_ms(10);
		}
		_delay_ms(100);

		for(uint8_t j = 0; j < 16; j++) {
			blink_col(j);
			_delay_ms(10);
		}
		for (int8_t i = 1; i >0 ; i--) {
			r_blink_col();
			_delay_ms(10);
		}
		_delay_ms(100);
		for(uint8_t j = 0; j < 16; j++) {
			blink_col_hori(j);
			_delay_ms(10);
		}
		for (int8_t i = 1; i >0 ; i--) {
			r_blink_col_hori();
			_delay_ms(10);
		}
		_delay_ms(100);
		for (uint8_t j = 0; j < 16; j++) {
			chase(j);
			_delay_ms(10);
		}
		_delay_ms(100);
		for (uint8_t j = 0; j < 16; j++) {
			chase_h(j);
			_delay_ms(10);
		}
		_delay_ms(100);
	 }
return 0;
}