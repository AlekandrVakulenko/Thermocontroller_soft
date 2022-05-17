#include <stdint.h>

//ассемблерные команды для С
#define _REG(addr) (*(volatile int*)(addr)) //запись в регистры I/O
#define _NOP __asm__ __volatile__ ("nop")   //все любят NOP

// Trigger
volatile uint32_t TrigCounter;
volatile uint32_t TrigCounterResult;
volatile uint8_t TrigCounterFlag;

// UART Receive
volatile uint8_t Uart_receive_timeout;
volatile uint8_t Uart_receive_buffer_len;
volatile uint8_t Uart_receive_buffer[5];
struct {
	uint8_t cmd;
	uint8_t argAH;
	uint8_t argAL;
	uint8_t argBH;
	uint8_t argBL;
} UART_CMD;


// UART Transmit
volatile uint16_t Uart_send_timer;
uint16_t Uart_send_period;
uint8_t Uart_ackn;
uint8_t Uart_request_flag;
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




//---------------------------------------------описание для C блока-------------------------------------------------------------------
//инициализация
void setup(void);

void GPIOinit(void);
void UARTinit(void);
void SPIinit(void);
void GPIOinterrupstinit(void);
void Timer1init(void);

void UartCMDexecute(void);


//вспомогательные функции
uint8_t ReadUARTsendtimer(uint16_t arg);
uint32_t ReadTrigCounterResult(void);
void UARTrecivetimeoutCheck(void);



//---------------------------------------------описание внешних функций ASM блока-----------------------------------------------------
//Параметры передаются через:
//для [u]int8_t              r24,             r22,             r20,             r18
//для [u]int16_t         r25:r24,         r23:r22,         r21:r20,         r19:r18
//для [u]int32_t r25:r24:r23:r22, r21:r20:r19:r18, r17:r16:r15:r14, r13:r12:r11:r10
//-------------
//ответ функции:
//для [u]int8_t		    r24
//для [u]int16_t		r25:r24
//для [u]int32_t		r25:r24:r23:r22
