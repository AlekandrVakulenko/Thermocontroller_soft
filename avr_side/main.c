//----------------------------------------------------------------------------
//ATmega328PB
//CPU_FREQ = 20 MHz
//----------------------------------------------------------------------------
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "main.h"
#include "utilities.h"
#include "Defines.h"
#include "DAC_ADC.h"
#include "PID.h"
//----------------------------------------------------------------------------


uint8_t timer_clk = 0;
uint16_t adc_filtered_value = 0;
uint16_t voltageout_d = 0;
uint32_t trig_last_value = 0;

uint16_t Temp_measured = 0;
float Temp_measured_f = 0;
uint16_t Temp_setpoint = 0;
float Temp_setpoint_f = 0;
uint16_t Temp_Gsetpoint = 0;
float Temp_Gsetpoint_f = 0;


//----------------------------------------------------------------------------
//----------------------------------Main--------------------------------------
//----------------------------------------------------------------------------
int main(void){
	setup(); //CPU init

	while(1){

		if (timer_clk == 1){
			timer_clk = 0;
			adc_filtered_value = ADC_read();
			Temp_measured_f = (float)adc_filtered_value/65536*5 * 58.5 + 189.9;
			Temp_measured = (uint16_t)(Temp_measured_f*100);
			
			
			voltageout_d = PID_func(&Temp_setpoint_f, &Temp_measured_f);
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

		if (ReadUARTsendtimer(Uart_send_period)){ //It's time to send data to PC
			if ((Uart_ackn == 0) & (Uart_request_flag == 1)){
				for (int8_t i = sizeof(UART_SEND_PACKET)-1; i>=0; --i){
					while(!(UCSR0A & 0b00100000)){_NOP;}
					UDR0 = UART_output_buffer.buf[i];
				}
				Uart_ackn = 1;
				Uart_request_flag = 0;
			}
		}

		UARTrecivetimeoutCheck();
		if (Uart_receive_buffer_len == 5){ //FIXME: where is a way to receive a new byte after IF condition is TRUE
			cli();
			UART_CMD.cmd = Uart_receive_buffer[0];
			UART_CMD.argAH = Uart_receive_buffer[1];
			UART_CMD.argAL = Uart_receive_buffer[2];
			UART_CMD.argBH = Uart_receive_buffer[3];
			UART_CMD.argBL = Uart_receive_buffer[4];
			Uart_receive_buffer_len = 0;
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

	Uart_send_timer = 0;
	Uart_send_period = 80; //x1.25ms (100ms)
	Uart_receive_timeout = 0;
	Uart_receive_buffer_len = 0;
	Uart_receive_buffer[0] = 0;
	Uart_ackn = 0;
	Uart_request_flag = 0;

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
	if (UART_CMD.cmd == 0x00){
		//no operation
		} else if (UART_CMD.cmd == 0x01) {

		} else if (UART_CMD.cmd == 0x02) {

		} else if (UART_CMD.cmd == 0x03) {

		} else if (UART_CMD.cmd == 0x04) {

		} else if (UART_CMD.cmd == 0x05) {
			uint16_t Value = (uint16_t)(UART_CMD.argAH<<8)+(uint16_t)(UART_CMD.argAL);
			if (Value>8000){Value = 8000;}
			if (Value<8){Value = 8;}
			Uart_send_period = Value;
			cli();
			Uart_send_timer = 0;
			sei();
		} else if (UART_CMD.cmd == 0x06) {
			Uart_ackn = 0;
		} else if (UART_CMD.cmd == 0x07) {

		} else if (UART_CMD.cmd == 0x08) {
			Temp_setpoint = (uint16_t)(UART_CMD.argAH<<8)+(uint16_t)(UART_CMD.argAL);
			Temp_setpoint_f = ((float)Temp_setpoint)/100;
		} else if (UART_CMD.cmd == 0x09) {
			voltageout_d = (uint16_t)(UART_CMD.argAH<<8)+(uint16_t)(UART_CMD.argAL);
			DAC_set(voltageout_d);
		} else if (UART_CMD.cmd == 0x0A) {
			Uart_request_flag = 1;
		} else if (UART_CMD.cmd == 0x0B) {

		} else if (UART_CMD.cmd == 0x0C) {

		} else if (UART_CMD.cmd == 0x0D) {

		} else if (UART_CMD.cmd == 0x0E) {

		} else {
		//no operation
	}
}


//----------------------------------------------------------------------------
//-----------------------16 bit read with protection--------------------------
//----------------------------------------------------------------------------
uint8_t ReadUARTsendtimer(uint16_t arg){
	cli();
	uint8_t output = (uint8_t) (Uart_send_timer >= arg); //protection while 16bits read
	if (output) {Uart_send_timer = 0;}
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
	uint8_t output = (Uart_receive_timeout >= 4 && Uart_receive_buffer_len > 0 && Uart_receive_buffer_len != 5);
	if (output) {
		Uart_receive_buffer_len = 0;
		Uart_receive_timeout = 0;
	}
	sei();
}


//----------------------------------------------------------------------------
//-----------------------------Interrupts-------------------------------------
//----------------------------------------------------------------------------
ISR(TIMER1_COMPA_vect){ //ADC start, inc TrigCounter, inc UARTsendtimer
	timer_clk = 1;
	TrigCounter++;
	Uart_send_timer++;
	Uart_receive_timeout++;
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
	if (Uart_receive_buffer_len>=5){
		Uart_receive_buffer_len = 0;
	}
	//записать новые данные по месту счетчека буфера
	Uart_receive_buffer[Uart_receive_buffer_len] = UDR0;
	Uart_receive_buffer_len++;

	Uart_receive_timeout = 0;
}

