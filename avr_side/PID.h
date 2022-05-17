#ifndef PID_H_
#define PID_H_

uint16_t PID_func(float* Temp_setpoint_f, float *Temp_measured_f);

typedef union {
	int32_t f;
	uint8_t buf[4];
} FLOAT_BUF;

int32_t get_derivative(void);

#endif /* PID_H_ */