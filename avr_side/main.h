#include <stdint.h>

//������������ ������� ��� �
#define _REG(addr) (*(volatile int*)(addr)) //������ � �������� I/O
#define _NOP __asm__ __volatile__ ("nop")   //��� ����� NOP


//---------------------------------------------�������� ��� C �����-------------------------------------------------------------------
//�������������
void setup(void);
void GPIOinit(void);
void UARTinit(void);
void SPIinit(void);
void GPIOinterrupstinit(void);
void Timer1init(void);

void UartCMDexecute(void);

void DAC_set(uint16_t arg);
uint16_t ADC_read(void);
uint16_t ADC_filter(void);
float temp_speed(void);

void Led_intro();

//��������������� �������
extern void DelayAsmX5(uint16_t arg); //��������� ����� x5 ������ ��� ��������           //----
uint8_t ReadUARTsendtimer(uint16_t arg);
uint32_t ReadTrigCounterResult(void);
void UARTrecivetimeoutCheck(void);



//---------------------------------------------�������� ������� ������� ASM �����-----------------------------------------------------
//��������� ���������� �����:
//��� [u]int8_t              r24,             r22,             r20,             r18
//��� [u]int16_t         r25:r24,         r23:r22,         r21:r20,         r19:r18
//��� [u]int32_t r25:r24:r23:r22, r21:r20:r19:r18, r17:r16:r15:r14, r13:r12:r11:r10
//-------------
//����� �������:
//��� [u]int8_t		    r24
//��� [u]int16_t		r25:r24
//��� [u]int32_t		r25:r24:r23:r22
