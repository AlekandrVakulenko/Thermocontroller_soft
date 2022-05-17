
#include <stdint.h>


float temp_error = 0;
float temp_error_Z1 = 0;
float filter = 0.2;
float derivative = 0;
float derivative_Z1 = 0;
float temp_error_sum = 0;
float PID_out = 0;



uint16_t PID_func(float* Temp_setpoint_f, float *Temp_measured_f){

	temp_error = (*Temp_setpoint_f - *Temp_measured_f);
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


	return (uint16_t)PID_out;
}

