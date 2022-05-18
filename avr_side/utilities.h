#ifndef UTILITIES_H_
#define UTILITIES_H_


extern void DelayAsmX5(uint16_t arg); //принимает число x5 тактов для ожидания  

void Delay100ms();
void Led_intro();
void Led_control_G2OR(uint8_t G2, uint8_t O, uint8_t R);

#endif /* UTILITIES_H_ */