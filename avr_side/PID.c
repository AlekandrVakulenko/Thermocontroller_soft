#include <stdint.h>
#include <stdlib.h>
#include "Defines.h"


float temp_error = 0;
float temp_error_Z1 = 0;
float filter = 1;
float derivative = 0;
float derivative_Z1 = 0;
float temp_error_sum = 0;
float PID_out = 0;

struct {
	float P;
	float I;
	float D;
	float Gain;
} PID_coeff = {900, 1.4, 1400, 0.5};


uint16_t PID_func(float* Temp_setpoint_f, float *Temp_measured_f){
	static float Temp_setpoint_f_Z1 = 0;
	static uint16_t PID_counter1 = 0, PID_counter2 = 0;
	static float error_array[16];

	PID_counter1++;
	PID_counter1 &= 0b00001111; //16
	PID_counter2 = PID_counter1 + 1;
	PID_counter2 &= 0b00001111; //16

	temp_error = (*Temp_setpoint_f - *Temp_measured_f);
	error_array[PID_counter1] = temp_error;
	if (abs(*Temp_setpoint_f - Temp_setpoint_f_Z1) > 0.1){
		for (int8_t i = 0; i<16; ++i){
			error_array[i] = temp_error;
		}
	}
	Temp_setpoint_f_Z1 = *Temp_setpoint_f;

	//derivative = temp_error - temp_error_Z1;
	//derivative = filter*derivative + (1-filter)*derivative_Z1;
	derivative = error_array[PID_counter1] - error_array[PID_counter2];
	derivative = filter * derivative + (1-filter) * derivative_Z1;
	//if (derivative > 5) derivative = 0.2;
	//if (derivative < -5) derivative = -0.2;
	derivative_Z1 = derivative;

	temp_error_Z1 = temp_error;
	temp_error_sum += temp_error;
	if (temp_error_sum > 600) temp_error_sum = 600;
	if (temp_error_sum < -50) temp_error_sum = -50;

	float proportional = temp_error*PID_coeff.P;
	if (proportional > 2000) proportional = 2000;
	if (proportional < -50) proportional = -50;

	float integral = temp_error_sum*PID_coeff.I;
	if (integral > 2000) integral = 2000;
	if (integral < -50) integral = -50;

	float differential = derivative*PID_coeff.D;
	if (differential > 1000) integral = 1000;
	if (differential < -200) integral = -200;


	PID_out = proportional + integral + differential;

	PID_out *= PID_coeff.Gain;

	if (PID_out < 0) PID_out = 0;
	if (PID_out > 2048) PID_out = 2048;

	if (*Temp_measured_f > 395) PID_out = 0; //first protection

	return (uint16_t)PID_out;
}

