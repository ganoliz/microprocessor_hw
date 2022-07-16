//
// Smpl_PWM_Music : use PWM to generate musical notes
//
// Output : PWM0 (GPB11)

#include <stdio.h>																											 
#include "NUC1xx.h"
#include "DrvSYS.h"
#include "note_freq.h"

#define  P125ms 125000
#define  P190ms 190000
#define  P250ms 250000

void PWM4_Init(void)
{
	PWMB->CSR.CSR0 = 0; // Divider
	PWMB->PPR.CP01 = 10; // prescaler
	
	PWMB->PCR.CH0MOD = 1; // Auto-reload Mode.
	
	PWMB->CMR0 = 1200000;  // PWM Group A/B Comparator Register 
	PWMB->CNR0 = 2272700;  // PWM Group A/B Counter Register 
	
	PWMB->POE.PWM0 = 1;  // PWM0: PWM output
	PWMB->PCR.CH0EN = 1;  // PWM Timer Starting.
}

void PWM_Freq(unsigned int freq, unsigned char duty_cycle)
{
	static unsigned short M, N;
	
	if(freq==0){
		M=0;
		N=0;
	}else{
		N = 50000000/(freq*22);   //high or low music high
		M =N/100*duty_cycle ;  
	}
	PWMB->CMR0 = M-1;
	PWMB->CNR0 = N-1;
}

int main (void)
{
  uint8_t i, j, duty_cycle;
	
  uint16_t music[147]={ 
		A5,A5,C6,C6,D6,D6,E6,E6,F6,F6,E6,E6,D6,D6,B5,B5,G5,G5,A5,A5,B5,B5,C6,C6,A5,A5,A5,A5,G5u,G5u,
		A5,A5,B5,B5,G5u,G5u,E5,E5,A5,A5,C6,C6,D6,D6,E6,E6,F6,F6,E6,E6,D6,D6,B5,B5,0,G5,G5,A5,A5,
    B5,B5,C6,C6,B5,B5,A5,A5,G5u,G5u,F5u,F5u,G5u,G5u,A5,A5,A5,A5,0,A5,
		G6,G6,G6,G6,F6,F6,E6,E6,D6,D6,B5,B5,G5,G5,A5,0,B5,0,C6,0,A5,0,A5,0,G5u,0,A5,
		0,B5,0,G5u,0,E5,0,G6,0,G6,0,F6u,0,E6,0,D6,0,B5,0,G5,0,A5,0,B5,0,C6,0,B5,0,A5,0,F5u,0,G5u,0,G5,0,G5,0,G5};    //3
	
		uint32_t pitch[147]={
			P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,
		  P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,
			P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,
			P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,
			P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms
			,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms
			,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms
			,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms
			,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms,P190ms
		};
	UNLOCKREG();
	DrvSYS_Open(50000000);// set System Clock to run at 50MHz.
	SYSCLK->CLKSEL0.STCLK_S = 0x3; 
	SYSCLK->CLKSEL2.PWM45_S = 2 ;   // PWM clock source
	SYSCLK->APBCLK.PWM45_EN = 1; 
	SYS->ALTMFP.PB11_PWM4 = 1; 
	SYS->GPBMFP.TM3_PWM4 = 1;  
	LOCKREG();

	PWM4_Init();
	
	while(1)
	{
		  for(i=0;i<147;i++)
  {
        PWM_Freq(music[i],80);		// Write your code here
		    DrvSYS_Delay(pitch[i]);
	}
}
}