#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Defines.h"


union {
	uint32_t DWORD;
	uint8_t CHAR[4];
} spi_buffer;


void DAC_set(uint16_t arg){
	if (arg >= 4096){
		arg = 4095;
	}
	spi_buffer.DWORD = (uint32_t)(arg << 2);
	
	SPCR1 = 0b01010101; //FOR DAC
	_NOP;
	cli();
	DAC_CSClear;
	for (int8_t i=1; i>=0; --i){
		SPDR1 = spi_buffer.CHAR[i];
		while (!(SPSR1 & 0b10000000)){_NOP;}
		spi_buffer.CHAR[i] = SPDR1;
	}
	DAC_CSSet;
	sei();
}



uint16_t ADC_read(void){
	SPCR1 = 0b01010101; //SPI init for DAC
	_NOP;
	cli();
	ADC_CSClear;
	for (int8_t i=2; i>=0; --i){
		SPDR1 = spi_buffer.CHAR[i];
		while (!(SPSR1 & 0b10000000)){_NOP;}
		spi_buffer.CHAR[i] = SPDR1;
	}
	ADC_CSSet;
	sei();
	return (uint16_t)((spi_buffer.DWORD >> 4) & 0x0000FFFF);
}


uint8_t adc_buf_clk = 0;
uint16_t adc_buf[32];
uint16_t ADC_filter(){
	adc_buf[adc_buf_clk] = ADC_read();
	adc_buf_clk++;
	adc_buf_clk = adc_buf_clk & 0b00011111;
	
	uint32_t midvalue = 0;
	for (uint8_t i = 0; i<32; ++i)
	{
		midvalue += adc_buf[i];
	}
	return (uint16_t)(midvalue >> 5);
}

float temp_speed(){
	float delta_V = (float)adc_buf[0] - (float)adc_buf[31];
	float delta_T = delta_V/65536*5 * 58.5;
	delta_T /= 0.05;
	return delta_T;
}


extern void DelayAsmX5(uint16_t arg); //принимает число x5 тактов для ожидания    
void Delay100ms(){
	for (uint8_t i=0; i<10; i++){ //100 ms
		DelayAsmX5( (uint16_t)4 * (uint16_t)10000); //10 ms
	}
}


void Led_intro(){
	Green_1_OFF;
	Green_2_OFF;
	Orange_OFF;
	RedOFF;
	Delay100ms();
	
	Green_1_ON;
	Delay100ms();
	Green_2_ON;
	Delay100ms();
	Orange_ON;
	Delay100ms();
	RedON;
	Delay100ms();
	
	Green_1_OFF;
	Delay100ms();
	Green_2_OFF;
	Delay100ms();
	Orange_OFF;
	Delay100ms();
	RedOFF;	
	
}










