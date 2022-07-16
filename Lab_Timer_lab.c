#include "NUC1xx.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvTIMER.h"
#include "Driver\DrvGPIO.h"
#define GPIO_OFFSET 0x40
int RED =255;
int GREEN = 255;
int BLUE = 255;

typedef enum{   
	E_GPIOA=0, 
	E_GPIOB,
	E_GPIOC, 
	E_GPIOD
}GPIO_GROUP;

typedef enum{
	E_INPUT=0, 
	E_OUTPUT,
	E_OPEN_DRAIN,
	E_QUASI
}GPIO_MODE;

void TMR0_IRQHandler1(void)   
{
  static unsigned int percentage = 0;
	
	// Brighten RGB led
	if(percentage < RED){
		// RED
		DrvGPIO_SetBit(E_GPA, 12);
		DrvGPIO_SetBit(E_GPA, 13);
		DrvGPIO_ClrBit(E_GPA, 14);
	}
	else if(percentage < GREEN+RED){
		// GEEEN
		DrvGPIO_SetBit(E_GPA, 12);
		DrvGPIO_ClrBit(E_GPA, 13);
		DrvGPIO_SetBit(E_GPA, 14);
	}
	else if(percentage < RED+GREEN+BLUE){
		// BLUE
		DrvGPIO_ClrBit(E_GPA, 12);
		DrvGPIO_SetBit(E_GPA, 13);
		DrvGPIO_SetBit(E_GPA, 14);
	}
	else if(percentage>=RED+GREEN+BLUE){
		percentage = 0;
	}
	percentage++;
	TIMER0->TISR.TIF = 1;		// clear Interrupt flag

}

void TIMER0_Init(void)
{
	// Timer Clock Source
	SYSCLK->CLKSEL1.TMR0_S = 2 ; // Clock source from HCLK.
	SYSCLK->APBCLK.TMR0_EN = 1;	//Enable Timer0 clock source
	
	// Enable Timer0 Interrupt
	NVIC_EnableIRQ(8);	// Set ISER to enable Timer Interrupt Service 
	
	// Setting Mode
		// 00 -> One-shot
		// 01 -> Periodic
	  // 10 -> Toggle
	  // 11 -> Continuous counting mode
	TIMER0->TCSR.MODE = 1;
	
	// Setting Prescale
	TIMER0->TCSR.PRESCALE = 5 ;
	
	// Data Load Enable
	TIMER0->TCSR.TDR_EN = 1;
	
	// Counter Mode Enable Bit
	TIMER0->TCSR.CTB = 0;          //set to 0 if it is not event counter
	
	// Enable interrupt
	TIMER0->TCSR.IE = 1;	
	
	// Timer Compared value
	TIMER0->TCMPR = 0x000011;
	
	
	
	TIMER0->TCSR.CRST = 1;	
	TIMER0->TCSR.CEN = 1;
}

void GPIO_Open(GPIO_GROUP group, unsigned int pin, GPIO_MODE mode)
{
	volatile unsigned int *reg_ptr;
	reg_ptr = (unsigned int *)(GPIO_BASE + group * GPIO_OFFSET);
	                                                              
	*reg_ptr |= (unsigned int)(mode << (pin << 1));   	                                                  	                                                 
}

int main(void)
{
	int i, j;
	static int R, G, B;

	int Color_Tab[7] = {
											0xFF0000,  // Red
											0xFF4500,  // Orange
											0xFFFF00,  // Yellow
											0x00FF00,  // Green
											0x00BFFF,  // Blue
											0x0000FF,  // Deep blue
											0x4B0082   // purple.
										}; // 7 colors 
	UNLOCKREG();
	DrvSYS_Open(50000000);// set System Clock to run at 50MHz 
	LOCKREG();
	
	GPIO_Open(E_GPIOA, 12, E_OUTPUT);
	GPIO_Open(E_GPIOA, 13, E_OUTPUT);
	GPIO_Open(E_GPIOA, 14, E_OUTPUT);
	
	// Init Timer.
	TIMER0_Init();

	while(1){

		// Change colors
		for(i=0; i<7; i++){
			
			R = (Color_Tab[i]&0xff0000) >> 16;
			G = (Color_Tab[i]&0x00ff00) >> 8;
		  B = (Color_Tab[i]&0x0000ff);		
			
			RED = R*100 /(256+256+256);
			GREEN = G*100 /(256+256+256);
			BLUE = B*100 /(256+256+256);
			
			for(j=0; j<1000; j++)
			DrvSYS_Delay(10000); //us
		}
	}
}


