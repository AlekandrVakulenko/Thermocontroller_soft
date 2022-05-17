#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Defines.h"
#include "utilities.h"


  
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










