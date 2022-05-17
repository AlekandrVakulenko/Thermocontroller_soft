//----------------------------------------------------------------------------
//ATmega328PB
//CPU_FREQ = 20 MHz
//----------------------------------------------------------------------------
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "main.h"
#include "Defines.h"
#include "DAC_ADC.h"
//----------------------------------------------------------------------------

volatile uint32_t TrigCounter;
volatile uint32_t TrigCounterResult;
volatile uint8_t TrigCounterFlag;
union {
	uint32_t DWORD;
	uint8_t CHAR[4];
} TrigCounterToSend;

volatile uint16_t UARTsendtimer;
volatile uint16_t UARTsendperiod;
volatile uint8_t UARTrecivetimeout;
volatile uint8_t UartReciveBufferLen;
volatile uint8_t UartReciveBuffer[5];
volatile uint8_t UARTackn;
volatile uint8_t RequestFlag;
struct {
	uint8_t cmd;
	uint8_t argAH;
	uint8_t argAL;
	uint8_t argBH;
	uint8_t argBL;
} UARTdata;


typedef struct UART_send_packet{
	uint16_t Temp_cK; //2
	uint16_t Temp_sp_cK; //2
	uint16_t Temp_gsp_cK; //2
	uint16_t Vout_d; //2
	uint16_t Vin_d; //2
	uint32_t trig_time; //4
	uint8_t serv1; //1
	uint8_t serv2; //1
	uint8_t serv3; //1
	uint8_t serv4; //1
} UART_SEND_PACKET; //18 bytes

union {
	UART_SEND_PACKET data;
	uint8_t buf[sizeof(UART_SEND_PACKET)];
} UART_output_buffer;

uint8_t adc_read_flag = 0;
uint16_t adc_filtered_value = 0;
uint16_t voltageout_d = 0;
uint32_t trig_last_value = 0;

uint16_t Temp_measured = 0;
float Temp_measured_f = 0;
uint16_t Temp_setpoint = 0;
float Temp_setpoint_f = 0;
uint16_t Temp_Gsetpoint = 0;
float Temp_Gsetpoint_f = 0;

float temp_error = 0;
float temp_error_Z1 = 0;
float filter = 0.2;
float derivative = 0;
float derivative_Z1 = 0;
float temp_error_sum = 0;
float PID_out = 0;
//----------------------------------------------------------------------------
//----------------------------------Main--------------------------------------
//----------------------------------------------------------------------------
int main(void){
	setup(); //CPU init

	while(1){

		if (adc_read_flag == 1){
			adc_read_flag = 0;
			adc_filtered_value = ADC_filter();
			Temp_measured_f = (float)adc_filtered_value/65536*5 * 58.5 + 189.9;
			Temp_measured = (uint16_t)(Temp_measured_f*100);
			temp_error = (Temp_setpoint_f - Temp_measured_f);
			derivative = temp_error - temp_error_Z1;
			derivative = filter*derivative + (1-filter)*derivative_Z1;
			derivative_Z1 = derivative;
			temp_error_Z1 = temp_error;
			temp_error_sum += temp_error;
			if (temp_error_sum > 800) temp_error_sum = 800;
			if (temp_error_sum < -800) temp_error_sum = -800;
			PID_out = temp_error*800 + temp_error_sum*1.5 + (derivative)*80000;
			if (PID_out < 0) PID_out = 0;
			if (PID_out > 2048) PID_out = 2048;
			voltageout_d = (uint16_t) PID_out;
			DAC_set(voltageout_d);
		}

		if (TrigCounterFlag == 1){
			trig_last_value = ReadTrigCounterResult();
		}

		UART_output_buffer.data.Temp_cK = Temp_measured;
		UART_output_buffer.data.Temp_sp_cK = Temp_setpoint;
		UART_output_buffer.data.Temp_gsp_cK = Temp_Gsetpoint;
		UART_output_buffer.data.Vout_d = voltageout_d;
		UART_output_buffer.data.Vin_d = adc_filtered_value; //unused (-2)
		UART_output_buffer.data.trig_time = trig_last_value; //too long (-2)
		UART_output_buffer.data.serv1 = 1;
		UART_output_buffer.data.serv2 = 2;
		UART_output_buffer.data.serv3 = 3; //unused (-1)
		UART_output_buffer.data.serv4 = 4; //unused (-1)

		if (ReadUARTsendtimer(UARTsendperiod)){ //It's time to send data to PC
			if ((UARTackn == 0) & (RequestFlag == 1)){
				for (int8_t i = sizeof(UART_SEND_PACKET)-1; i>=0; --i){
					while(!(UCSR0A & 0b00100000)){_NOP;}
					UDR0 = UART_output_buffer.buf[i];
				}
				UARTackn = 1;
				RequestFlag = 0;
			}
		}

		UARTrecivetimeoutCheck();
		if (UartReciveBufferLen == 5){ //FIXME: where is a way to receive a new byte after IF condition is TRUE
			cli();
			UARTdata.cmd = UartReciveBuffer[0];
			UARTdata.argAH = UartReciveBuffer[1];
			UARTdata.argAL = UartReciveBuffer[2];
			UARTdata.argBH = UartReciveBuffer[3];
			UARTdata.argBL = UartReciveBuffer[4];
			UartReciveBufferLen = 0;
			sei();
			UartCMDexecute();
		}


	} //while(1)
} //main
//----------------------------





//----------------------------------------------------------------------------
//--------------------------Global Initialization-----------------------------
//----------------------------------------------------------------------------
void setup(void){
	GPIOinit();
	UARTinit();
	SPIinit();
	GPIOinterrupstinit();
	Timer1init();

	TrigCounter = 0;
	TrigCounterResult = 0;
	TrigCounterFlag = 0;
	TrigCounterToSend.DWORD = (uint32_t) 0;

	UARTsendtimer = 0;
	UARTsendperiod = 80; //x1.25ms (100ms)
	UARTrecivetimeout = 0;
	UartReciveBufferLen = 0;
	UartReciveBuffer[0] = 0;
	UARTackn = 0;
	RequestFlag = 0;

	ADC_CSSet;
	DAC_CSSet;
	Current_SW_OFF;
	Power_out_OFF;
	DAC_set(0);
	Led_intro();

	sei();
}

//----------------------------------------------------------------------------
//----------------------------UartCMDexecute----------------------------------
//----------------------------------------------------------------------------
void UartCMDexecute(void){ //переделать в SWITCH CASE
	if (UARTdata.cmd == 0x00){
		//no operation
		} else if (UARTdata.cmd == 0x01) {

		} else if (UARTdata.cmd == 0x02) {

		} else if (UARTdata.cmd == 0x03) {

		} else if (UARTdata.cmd == 0x04) {

		} else if (UARTdata.cmd == 0x05) {
		uint16_t Value = (uint16_t)(UARTdata.argAH<<8)+(uint16_t)(UARTdata.argAL);
		if (Value>8000){Value = 8000;}
		if (Value<8){Value = 8;}
		UARTsendperiod = Value;
		cli();
		UARTsendtimer = 0;
		sei();
		} else if (UARTdata.cmd == 0x06) {
		UARTackn = 0;
		} else if (UARTdata.cmd == 0x07) {

		} else if (UARTdata.cmd == 0x08) {
		Temp_setpoint = (uint16_t)(UARTdata.argAH<<8)+(uint16_t)(UARTdata.argAL);
		Temp_setpoint_f = ((float)Temp_setpoint)/100;
		} else if (UARTdata.cmd == 0x09) {
		voltageout_d = (uint16_t)(UARTdata.argAH<<8)+(uint16_t)(UARTdata.argAL);
		DAC_set(voltageout_d);
		} else if (UARTdata.cmd == 0x0A) {
		RequestFlag = 1;
		} else if (UARTdata.cmd == 0x0B) {

		} else if (UARTdata.cmd == 0x0C) {

		} else if (UARTdata.cmd == 0x0D) {

		} else if (UARTdata.cmd == 0x0E) {
		TrigCounterToSend.DWORD = 0; //FIXME unknown CMD
		} else {
		//no operation
	}
}


//----------------------------------------------------------------------------
//-----------------------16 bit read with protection--------------------------
//----------------------------------------------------------------------------
uint8_t ReadUARTsendtimer(uint16_t arg){
	cli();
	uint8_t output = (uint8_t) (UARTsendtimer >= arg); //protection while 16bits read
	if (output) {UARTsendtimer = 0;}
	sei();
	return output;
}

uint32_t ReadTrigCounterResult(void){
	cli();
	uint32_t output = TrigCounterResult; //protection while 16 read
	TrigCounterResult = 0;
	sei();
	TrigCounterFlag = 0;
	return output;
}

void UARTrecivetimeoutCheck(void){
	cli();
	uint8_t output = (UARTrecivetimeout >= 4 && UartReciveBufferLen > 0 && UartReciveBufferLen != 5);
	if (output) {
		UartReciveBufferLen = 0;
		UARTrecivetimeout = 0;
	}
	sei();
}


//----------------------------------------------------------------------------
//-----------------------------Interrupts-------------------------------------
//----------------------------------------------------------------------------
ISR(TIMER1_COMPA_vect){ //ADC start, inc TrigCounter, inc UARTsendtimer
	adc_read_flag = 1;
	TrigCounter++;
	UARTsendtimer++;
	UARTrecivetimeout++;
}

ISR(INT0_vect){ // reset TrigCounter on fall, save TrigCounter value on rise
	if ((PIND & 0b00000100) == 0){
		TrigCounter = 0;
		} else {
		TrigCounterResult = TrigCounter;
		TrigCounterFlag = 1;
	}
}

ISR(USART0_RX_vect){ //UART reading into Recive buffer

	//если счетчик буфера больше 5, то сбросить его на 0
	if (UartReciveBufferLen>=5){
		UartReciveBufferLen = 0;
	}
	//записать новые данные по месту счетчека буфера
	UartReciveBuffer[UartReciveBufferLen] = UDR0;
	UartReciveBufferLen++;

	UARTrecivetimeout = 0;
}

