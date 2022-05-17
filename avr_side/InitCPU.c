#include <avr/io.h>

//-------распиновка микроконтроллера на плате----------
//Порт B
// B0 - (input)
// B1 - (input)
// B2 - CONST_HIGH (output/high)
// B3 - prog_MOSI (output)
// B4 - prog_MISO (input)
// B5 - prog_CLK (output)
// B6 - XTAL1
// B7 - XTAL2
//  DDRB = 0b00101100;
// PORTB = 0b00000100;

//Порт C
// C0 - SPI_MISO (input)
// C1 - SPI_CLK (output)
// C2 - SS_DAC (output)
// C3 - SS_ADC (output)
// C4 - power_out_SW (output)
// C5 - current_SW (output)
// C6 - RESET
//  DDRC = 0b0111110;
// PORTC = 0b0001100;

//Порт D
// D0 - UART_RX
// D1 - UART_TX
// D2 - Trig input (INT1) (input)
// D3 - (input)
// D4 - toLED_1 (output) GREEN_1
// D5 - toLED_2 (output) GREEN_2
// D6 - toLED_3 (output) ORANGE
// D7 - toLED_4 (output) RED
//  DDRD = 0b11110000;
// PORTD = 0b00000000;

//Порт E
// E0 - (input)
// E1 - (input)
// E2 - CONST_HIGH (output)
// E3 - SPI_MOSI (output)
//-----------------------------------------------------
//  DDRE = 0b1100;
// PORTE = 0b0100;

void GPIOinit(void){
	DDRB = 0b00101100;
	PORTB = 0b00000100;

	DDRC = 0b0111110;
	PORTC = 0b0001100;

	DDRD = 0b11110000;
	PORTD = 0b11110000;

	DDRE = 0b1100;
	PORTE = 0b0100;
}

void UARTinit(void){
	//230.4k-8-N-1
	UCSR0A = 0b00000010; //удвоение скорости(1)
	UCSR0B = 0b10011000;
	UCSR0C = 0b00000110;
	UBRR0L = 10;
	UBRR0H = 0;
}

void SPIinit(void){
	SPSR1 = 0b00000000;
	SPCR1 = 0b01010101; //FOR DAC
	SPCR1 = 0b01010001; //FOR ADC

	//SPDR1 = ; data reg
	//while (!(SPSR1 & 0b10000000)){_NOP;}
}

void Timer1init(void){
	TCCR1A = 0b00000000;
	TCNT1 = 0x0000;
	OCR1A = 24999;
	TIMSK1 = 0b00000010;
	TCCR1B = 0b00001001; //0b00001001 to start
}

void GPIOinterrupstinit(void){
	//INT1 and INT0
	EICRA = 0b00000001; //int0 to any edge
	EIMSK = 0b00000001; //enable int0
}





