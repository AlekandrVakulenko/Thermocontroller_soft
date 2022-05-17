#ifndef DEFINES_H_
#define DEFINES_H_

//FIXME: try to replace by name (#include <avr/io.h>)


//  *PINB = 0x03;
//  *DDRB = 0x04;
// *PORTB = 0x05;

//  *PINC = 0x06;
//  *DDRC = 0x07;
// *PORTC = 0x08;

//  *PIND = 0x09;
//  *DDRD = 0x0A;
// *PORTD = 0x0B;

//  *PINE = 0x0C;
//  *DDRE = 0x0D;
// *PORTE = 0x0E;

#define _NOP __asm__ __volatile__ ("nop")   //все любят NOP

//-----------------------------------------------------------

#define DAC_CSSet __asm__ __volatile__ ("SBI 0x08, 2")
#define DAC_CSClear __asm__ __volatile__ ("CBI 0x08, 2")

#define ADC_CSSet __asm__ __volatile__ ("SBI 0x08, 3")
#define ADC_CSClear __asm__ __volatile__ ("CBI 0x08, 3")

//-----------------------------------------------------------

#define Power_out_ON __asm__ __volatile__ ("SBI 0x08, 4")
#define Power_out_OFF __asm__ __volatile__ ("CBI 0x08, 4")

#define Current_SW_ON __asm__ __volatile__ ("SBI 0x08, 5")
#define Current_SW_OFF __asm__ __volatile__ ("CBI 0x08, 5")

//-----------------------------------------------------------

#define Green_1_OFF __asm__ __volatile__ ("SBI 0x0B, 4")
#define Green_1_ON __asm__ __volatile__ ("CBI 0x0B, 4")
#define Green_1_Toggle __asm__ __volatile__ ("SBI 0x09, 4")

#define Green_2_OFF __asm__ __volatile__ ("SBI 0x0B, 5")
#define Green_2_ON __asm__ __volatile__ ("CBI 0x0B, 5")
#define Green_2_Toggle __asm__ __volatile__ ("SBI 0x09, 5")

#define Orange_OFF __asm__ __volatile__ ("SBI 0x0B, 6")
#define Orange_ON __asm__ __volatile__ ("CBI 0x0B, 6")
#define Orange_Toggle __asm__ __volatile__ ("SBI 0x09, 6")

#define RedOFF __asm__ __volatile__ ("SBI 0x0B, 7")
#define RedON __asm__ __volatile__ ("CBI 0x0B, 7")
#define RedToggle __asm__ __volatile__ ("SBI 0x09, 7")

//-----------------------------------------------------------



#endif /* DEFINES_H_ */