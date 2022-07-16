
#include <stdio.h>																											 
#include "NUC1xx.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvGPIO.h"

//define 7seg codex here
#define SEG_0   0x82 
#define SEG_1   0xEE 

// Write your code

unsigned int SEG[10]={SEG_0, SEG_1, SEG_2, SEG_3, SEG_4, SEG_5, SEG_6, SEG_7, SEG_8, SEG_9}; 

	volatile unsigned int *reg_ptr;
// display an integer on four 7-segment LEDs

void SYS_Delay(unsigned int us)
{
		static unsigned char repeat;
	
		// If STCLK is 25M Hz.
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

void GPC_set(unsigned int group, unsigned int pin)
{
	reg_ptr = (unsigned int *)(0x50004000 + group * 0x40 + 0x08);
	*reg_ptr = (unsigned int)((1<<pin) | (0xf<<12));

}

void show_seven_segment(unsigned int place, unsigned int number)
{
	unsigned int temp,i;
	temp=SEG[number];
	
	for(i=0;i<8;i++){
	  if((temp&0x01)==0x01){		   	  
			DrvGPIO_SetBit(E_GPE,i);
	  }else{
			DrvGPIO_ClrBit(E_GPE,i);	
	  }	  
			temp=temp>>1;
	}
	GPC_set(E_GPC,3+place);	
}

void close_seven_segment(void)
{
	GPIOC->DOUT &= ~(0xF<<4);
}

int32_t main (void)
{
	int TEXT1[16];
	uint32_t i = 0;
	
	UNLOCKREG();
	DrvSYS_Open(50000000);// set System Clock to run at 50MHz. 
	SYSCLK->CLKSEL0.STCLK_S = 0x3; // STCLK Clock Source.
	LOCKREG();	
	
	while(1)
	{	
		// Write your code here
		
		show_seven_segment(1,4);
		SYS_Delay(4000);  // 4000 us = 4 ms
		
		show_seven_segment(2,3);
		SYS_Delay(4000);  // 4000 us = 4 ms
		
		show_seven_segment(3,2);
		SYS_Delay(4000);  // 4000 us = 4 ms
		
		show_seven_segment(4,1);
		SYS_Delay(4000);  // 4000 us = 4 ms
	}	

}
