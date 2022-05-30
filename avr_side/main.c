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


volatile uint8_t timer_clk = 0;
uint16_t adc_filtered_value = 0;
uint16_t voltageout_d = 0;
uint32_t trig_last_value = 0;


float Temp_measured_f = 0;
float Temp_setpoint_f = 295;
float Temp_ramp_target_f = 295;
float Temp_ramp_speed_f = 0; // K/min
uint8_t ramping_flag = 0;
uint8_t ramp_direction = 0; // 1 - heat, 0 - cool
uint8_t heating_flag = 0;


//----------------------------------------------------------------------------
//----------------------------------Main--------------------------------------
//----------------------------------------------------------------------------
int main(void){
	setup(); //CPU init

	while(1){
		Led_control_G2OR(heating_flag, ramping_flag, (uint8_t) (voltageout_d > 0));

		//FIXME TODO: add global PID sign
		
		if (timer_clk == 1){
			static uint8_t local_timer = 0;
			timer_clk = 0;
			local_timer++;
			if (local_timer == 100){ //100*0.00125 = 0.125 s
				local_timer = 0;
				if (ramping_flag && heating_flag){
					float delta_temp = Temp_ramp_speed_f/480;
					if (ramp_direction){
						Temp_setpoint_f += delta_temp;
						if (Temp_setpoint_f >= Temp_ramp_target_f){
							ramping_flag = 0;
							Temp_setpoint_f = Temp_ramp_target_f;
						}
						} else {
						Temp_setpoint_f -= delta_temp;
						if (Temp_setpoint_f <= Temp_ramp_target_f){
							ramping_flag = 0;
							Temp_setpoint_f = Temp_ramp_target_f;
						}
					}
				}
			}

			adc_filtered_value = ADC_read();
			Temp_measured_f = (float)adc_filtered_value/65536*5 * 58.5 + 189.9;

			voltageout_d = PID_func(&Temp_setpoint_f, &Temp_measured_f);
			if (Temp_measured_f > 395) voltageout_d = 0; //second protection
			if (heating_flag){
				DAC_set(voltageout_d);
			}
			else {
				voltageout_d = 0;
			}
		}

		if (TrigCounterFlag == 1){
			trig_last_value = ReadTrigCounterResult();
		}

		//UART data preparing to send
		UART_output_buffer.data.Temp_cK = (uint16_t)(Temp_measured_f*100);
		UART_output_buffer.data.Temp_sp_cK = (uint16_t)(Temp_setpoint_f*100);
		UART_output_buffer.data.Temp_r_t_cK = (uint16_t)(Temp_ramp_target_f*100);
		UART_output_buffer.data.Vout_d = voltageout_d;
		UART_output_buffer.data.Vin_d = adc_filtered_value;
		UART_output_buffer.data.trig_time = trig_last_value; //too long (-2)
		UART_output_buffer.data.serv1 = heating_flag |
										ramping_flag << 1 |
										((uint8_t) (voltageout_d > 0)) << 2 |
										(PIND & 0b00000100) << 1;
		UART_output_buffer.data.serv2 = 2;
		UART_output_buffer.data.serv3 = 3;
		UART_output_buffer.data.serv4 = 4; //unused (-1)

		if (ReadUARTsendtimer(Uart_send_period)){ //It's time to send data to PC
			if ((Uart_ackn == 0) & (Uart_request_flag == 1)){
				for (int8_t i = sizeof(UART_SEND_PACKET)-1; i>=0; --i){
					while(!(UCSR0A & 0b00100000)){_NOP;}
					UDR0 = UART_output_buffer.buf[i];
				}
				Uart_ackn = 1;
				Uart_request_flag = 0;
				trig_last_value = 0; //crear after send
			}
		}

		UARTrecivetimeoutCheck();
		cli();
		if (Uart_receive_buffer_len == 5){
			UART_CMD.cmd = Uart_receive_buffer[0];
			UART_CMD.argAH = Uart_receive_buffer[1];
			UART_CMD.argAL = Uart_receive_buffer[2];
			UART_CMD.argBH = Uart_receive_buffer[3];
			UART_CMD.argBL = Uart_receive_buffer[4];
			Uart_receive_buffer_len = 0;
			UartCMDexecute();
		}
		sei();

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

	heating_flag = 0;

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

		} else if (UART_CMD.cmd == 0x05) { //SET sending speed
			uint16_t Value = (uint16_t)(UART_CMD.argAH<<8)+(uint16_t)(UART_CMD.argAL);
			if (Value>8000){Value = 8000;}
			if (Value<8){Value = 8;}
			Uart_send_period = Value;
			cli();
			Uart_send_timer = 0;
			sei();

		} else if (UART_CMD.cmd == 0x06) { //ackn
			Uart_ackn = 0;

		} else if (UART_CMD.cmd == 0x07) { //SET ramp target
			uint16_t ramp_temp_d = (uint16_t)(UART_CMD.argAH<<8)+(uint16_t)(UART_CMD.argAL);
			uint16_t ramp_speed_d = (uint16_t)(UART_CMD.argBH<<8)+(uint16_t)(UART_CMD.argBL);
			Temp_ramp_target_f = ((float) ramp_temp_d)/100;
			Temp_ramp_speed_f = ((float) ramp_speed_d)/100;
			if (Temp_setpoint_f < Temp_ramp_target_f){
				ramp_direction = 1;
			} else {
				ramp_direction = 0;
			}
			ramping_flag = 1;

		} else if (UART_CMD.cmd == 0x08) { //SET temp_set_point
			uint16_t value_d = (uint16_t)(UART_CMD.argAH<<8)+(uint16_t)(UART_CMD.argAL);
			Temp_setpoint_f = ((float) value_d)/100;
			ramping_flag = 0;

		} else if (UART_CMD.cmd == 0x09) { //SET output voltage
			voltageout_d = (uint16_t)(UART_CMD.argAH<<8)+(uint16_t)(UART_CMD.argAL);
			DAC_set(voltageout_d);
			ramping_flag = 0;

		} else if (UART_CMD.cmd == 0x0A) { // Data Request
			Uart_request_flag = 1;

		} else if (UART_CMD.cmd == 0x0B) { //Enable heater
			heating_flag = 1;
		} else if (UART_CMD.cmd == 0x0C) { //Disable heater
			heating_flag = 0;
		} else if (UART_CMD.cmd == 0x0D) { //Stop ramp
			ramping_flag = 0;
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

