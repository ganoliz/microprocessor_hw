#include "NUC1xx.h"
#include <string.h>
#include <stdlib.h>
#include "Driver\DrvSYS.h"
#include "Driver\DrvUART.h"
#include "Header_File\GPIO.h"
#include "Header_File\GPIO_Define.h"

#define Data_depth	128
#define LED_MASK (PIN_12 | PIN_13 | PIN_14 | PIN_15)
#define LED_GROUP E_GPIOC
#define LED0 PIN_12
#define LED1 PIN_13
#define LED2 PIN_14
#define LED3 PIN_15


typedef struct{
	unsigned char NO;
	const unsigned char *MSG;
}MSG_T;

MSG_T MSG_TABLE[] = {
	{0, (const unsigned char *)"HI, Welecome to use this system, Please Enter Mathematical formulas!"}
};

const unsigned int SEG_ARRAY[] = {SEG_0, SEG_1, SEG_2, SEG_3, SEG_4, SEG_5, SEG_6, SEG_7, SEG_8, SEG_9};

unsigned char *USER_OPERATION_PTR;
unsigned char USER_OPERATION[Data_depth];

int C=0;

void Data_Process(unsigned char *buf)
{
	volatile static int A, B;
	static unsigned int oper;
	static unsigned char *ptr;

	ptr = buf;
	
	// C = A (+/-) B
	while(*ptr!='+' && *ptr!='-')ptr++;
	oper = (*ptr=='+')? 1:0;
	*ptr = 0;
	A = atoi((const char *)buf);
	B = atoi((const char *)(ptr+1));  // string to int
	
	if(oper == 1){
		C = (A+B);
		GPIO_Output(LED_GROUP, LED0, ~LED0);
		GPIO_Output(LED_GROUP, LED1, LED1);
	}else{
		C = (A-B);
		GPIO_Output(LED_GROUP, LED0, LED0);
		GPIO_Output(LED_GROUP, LED1, ~LED1);
	}
	
}

void UART_INT_HANDLE(unsigned int data)
{
	static unsigned char buf;
		
	while(UART0->ISR.RDA_IF==1){
		buf = UART0->DATA;
		UART0->DATA = buf;

		while(UART0->ISR.THRE_IF==0){
		// While Writting Data End.
		}
		if(buf == 0x7f)
		{
			if(USER_OPERATION_PTR !=USER_OPERATION) USER_OPERATION_PTR--;
			return;
		}
		*(USER_OPERATION_PTR++) = buf;
		
		if(buf == 0x0d){
			*USER_OPERATION_PTR = 0;
			USER_OPERATION_PTR = USER_OPERATION;
			Data_Process(USER_OPERATION_PTR);
			UART0->DATA = 0x0a;
			while(UART0->ISR.THRE_IF==0){
			// While Writting Data End.
			}
		}
	}
}

void UART_SEND_STRING(unsigned char *buf, unsigned int lengh)
{
	static unsigned int count;
	count = 0;
	do{
			UART0->DATA = *(buf+count);
			while(UART0->ISR.THRE_IF==0){
			// While Writting Data End.
			}		
	}while(count++ != lengh);

	UART0->DATA = 0x0d; // Enter ASCII   
	while(UART0->ISR.THRE_IF==0){
	// While Writting Data End.
	}
	
	UART0->DATA = 0x0a; // Next Column ASCII
	while(UART0->ISR.THRE_IF==0){
	// While Writting Data End.
	}
}

void SYS_Delay(unsigned int us)
{
		static unsigned char repeat;
	
		// If sys clock is 25M Hz.
	  //
	  repeat = 25;

			SysTick->CTRL &= ~( 1 | 1 << 16 ); 
			SysTick->LOAD = us;
			SysTick->VAL  = 0;
			SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
		while(repeat--){
			/* Waiting for down-count to zero */
			while((SysTick->CTRL & (1 << 16)) == 0);
			SysTick->VAL  = 0;
		}
}



void UART0_INIT()
{
	volatile unsigned int *reg_ptr;
	
	// Pin_mux
	SYS->GPBMFP.UART0_RX = 1;
	SYS->GPBMFP.UART0_TX = 1;
	//
	  // SETING BAUD RATE. (115200)
	  // Please see datasheet.
		reg_ptr = (unsigned int *)(UART0_BASE +0x24);  // UA_BAUD's offset is 0x24. 
		*reg_ptr = 0x300000BE;
	//
		// Setting Mode to UART Function.
	  //
		reg_ptr = (unsigned int *)(UART0_BASE +0x30);  // UA_FUN_SEL's offset is 0x24. 
		*reg_ptr = 0x0UL;
	//
		// Setting Mode to UART Function.
	  //
		reg_ptr = (unsigned int *)(UART0_BASE +0x0C);  // UA_LCR's offset is 0x0c. 
		*reg_ptr = 0x3UL;  
		 
		reg_ptr = (unsigned int *)(UART0_BASE +0x04);  // UA_FUN_SEL's offset is 0x24. 
		*reg_ptr = 0x03;

		UART0->FCR.TFR =1;
  	UART0->FCR.RFR =1;		
}

void SEG_DISPLAY(void)
{
	static unsigned char Single_num, Ten_num, Hundred_num, Thousand_num;
	static int lC;
	while(1){
			lC = C;
			if( (C <= 9999)){
				
				if(C>=0) 
				  GPIO_Output(LED_GROUP, LED3, LED3);
				else
				{
					GPIO_Output(LED_GROUP, LED3, ~LED3);
					lC = ~(lC-1);
				}
				Thousand_num = lC/1000;
				Hundred_num = lC/100 - Thousand_num *10;
				Ten_num = lC/10 - Thousand_num *100 - Hundred_num * 10;
				Single_num = lC%10;
				GPIO_Output(LED_GROUP, LED2, LED2);
			}else{
				Thousand_num = 0;
				Hundred_num = 0;
				Ten_num = 0;
				Single_num = 0;
				GPIO_Output(LED_GROUP, LED2, ~LED2);
			}
		
			// Single.
			GPIO_Output(SEGMENT_CS_GROUP, SEG_CS_MASK, SEGMENT_1);
			GPIO_Output(SEGMENT_LED_GROUP, SEG_PIN_MASK, SEG_ARRAY[Single_num]);
			SYS_Delay(5000); // 5ms
			// Ten.
			GPIO_Output(SEGMENT_CS_GROUP, SEG_CS_MASK, SEGMENT_2);
			GPIO_Output(SEGMENT_LED_GROUP, SEG_PIN_MASK, SEG_ARRAY[Ten_num]);
			SYS_Delay(5000); // 5ms
			// Hundred.
			GPIO_Output(SEGMENT_CS_GROUP, SEG_CS_MASK, SEGMENT_3);
			GPIO_Output(SEGMENT_LED_GROUP, SEG_PIN_MASK, SEG_ARRAY[Hundred_num]);
			SYS_Delay(5000); // 5ms
			// Thousand.
			
			GPIO_Output(SEGMENT_CS_GROUP, SEG_CS_MASK, SEGMENT_4);
			GPIO_Output(SEGMENT_LED_GROUP, SEG_PIN_MASK, SEG_ARRAY[Thousand_num]);
			SYS_Delay(5000); // 5ms		
	}
}

int main(void)
{
	USER_OPERATION_PTR = USER_OPERATION;
	
	UNLOCKREG();
	DrvSYS_Open(50000000);// set System Clock to run at 50MHz.
	SYSCLK->CLKSEL0.STCLK_S = 0x3; // system tick's clock.
	// Setup UART0 clock source.
	SYSCLK->APBCLK.UART0_EN = 1;
	SYSCLK->CLKSEL1.UART_S = 0x3; 
	SYSCLK->CLKDIV.UART_N = 0; 
	LOCKREG();
	
	UART0_INIT();
	DrvUART_EnableInt(UART_PORT0, DRVUART_RDAINT, UART_INT_HANDLE);
	
	GPIO_Open(SEGMENT_LED_GROUP, SEG_PIN_MASK, E_OUTPUT);
	GPIO_Open(SEGMENT_CS_GROUP, SEG_CS_MASK | LED_MASK , E_OUTPUT);

	GPIO_Output(SEGMENT_CS_GROUP, SEG_CS_MASK, 0);
	GPIO_Output(LED_GROUP, LED_MASK, LED_MASK);

	UART_SEND_STRING((unsigned char *)MSG_TABLE[0].MSG, strlen((const char *)MSG_TABLE[0].MSG));
	
	SEG_DISPLAY();
}

void HardFault_Handler(void)
{
	while(1){
		//HardFault
	}

}



