#include "NUC1xx.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvTIMER.h"
#include "Driver\DrvGPIO.h"

#define GPIO_OFFSET 0x40

#define LED0 (0x1UL << 12)
#define LED1 (0x1UL << 13)
#define LED2 (0x1UL << 14)
#define LED3 (0x1UL << 15)

#define LED_MASK (LED3 | LED2 | LED1 | LED0)

unsigned int LED_MODE1[] = 
{
	LED0,
	LED1,
	LED2,
	LED3, 
	0xffffffff
};

unsigned int *LED_MODE[] = 
{
	LED_MODE1, 
	NULL
};

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

void GPIO_Open(GPIO_GROUP group, unsigned int pin, GPIO_MODE mode)
{
	// IO mode, GP_BA
	volatile unsigned int *reg_ptr;
	
	reg_ptr = (unsigned int *)(GPIO_BASE + group * GPIO_OFFSET); 
	*reg_ptr |= (unsigned int)(mode << (pin << 1));
}

void GPIO_Output(GPIO_GROUP group, unsigned int pinmask, unsigned int data)
{ 
	volatile unsigned int *reg_ptr;
	
	// GPIOA_PMD
	reg_ptr = (unsigned int *)(GPIO_BASE + group * 0x40 + 0xc); // 0x40 = GPIO_OFFSET
	*reg_ptr = ~pinmask;	
	
	// GPIOA_DOUT
	reg_ptr = (unsigned int *)(GPIO_BASE + group * 0x40 + 8); // 0x40 = GPIO_OFFSET
	*reg_ptr = data;

}

void 	LED_Write()
{
  int i, j;
	
	while(1){
		for(i=0; LED_MODE[i]!=NULL; i++){
			for(j=0; LED_MODE[i][j]!=~NULL; j++){
				GPIO_Output(E_GPIOC, LED_MASK, ~LED_MODE[i][j]);
				SYS_Delay(1000000/2);
			}
		}
	}
}

int main(void)
{
	volatile static int hclk, pll;
	
	UNLOCKREG();
	DrvSYS_Open(50000000);// set System Clock to run at 50MHz 
	SYSCLK->CLKSEL0.STCLK_S = 0x3;
	LOCKREG();

	hclk = DrvSYS_GetHCLKFreq();
	pll = DrvSYS_GetPLLClockFreq();
	
  GPIO_Open(E_GPIOC, 12, E_OUTPUT); // GPC12, 13, 14, 15.
  GPIO_Open(E_GPIOC, 13, E_OUTPUT); // GPC12, 13, 14, 15.
  GPIO_Open(E_GPIOC, 14, E_OUTPUT); // GPC12, 13, 14, 15.
  GPIO_Open(E_GPIOC, 15, E_OUTPUT); // GPC12, 13, 14, 15.
	
	LED_Write();
	
	while(1){
		// It is nothing to do.
	}
	
}


void HardFault_Handler(void)
{
	while(1){
		//HardFault
	}

}


