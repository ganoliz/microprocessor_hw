#include "NUC1xx.h"
#include "DrvSYS.h"
#include "GPIO.h"
#include "GPIO_Define.h"
#define PORT_OFFSET   0x40


unsigned char seg[4] = {0x0,0x0,0x0,0x0};

#define SEG_N0   0x82 
#define SEG_N1   0xEE 
#define SEG_N2   0x07 
#define SEG_N3   0x46 
#define SEG_N4   0x6A  
#define SEG_N5   0x52 
#define SEG_N6   0x12 
#define SEG_N7   0xE6 
#define SEG_N8   0x02 
#define SEG_N9   0x42


unsigned char SEG_BUF[16]={SEG_N0, SEG_N1, SEG_N2, SEG_N3, SEG_N4, SEG_N5, SEG_N6, SEG_N7, SEG_N8, SEG_N9}; 

volatile unsigned int *reg_ptr2,*reg_ptr3; 
volatile unsigned int *reg_ptr;
 int32_t result=0; 

int32_t DrvGPIO_SetBit(E_DRVGPIO_PORT port, int32_t i32Bit)
{
	GPIO_T * tGPIO;

    if ((i32Bit < 0) || (i32Bit > 16))
    {
        return E_DRVGPIO_ARGUMENT;
    }

	tGPIO = (GPIO_T *)((uint32_t)GPIOA + (port*PORT_OFFSET));  
  tGPIO->DMASK=0;
	tGPIO->DOUT |= (1 << i32Bit);

	return E_SUCCESS;
}

int32_t DrvGPIO_ClrBit(E_DRVGPIO_PORT port, int32_t i32Bit)
{
	GPIO_T * tGPIO;

    if ((i32Bit < 0) || (i32Bit > 16))
    {
        return E_DRVGPIO_ARGUMENT;
    }

	tGPIO = (GPIO_T *)((uint32_t)GPIOA + (port*PORT_OFFSET));  
  tGPIO->DMASK=0;
	tGPIO->DOUT &= ~(1 << i32Bit);

	return E_SUCCESS;    
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

void ADC_INIT(void)
{
	 /* reset ADC */
    SYS->IPRSTC2.ADC_RST = 1; 
    SYS->IPRSTC2.ADC_RST = 0;
	  
	  // ADC Enable
		ADC->ADCR.ADEN = 1; 
	
	// Pinmux
		SYS->GPAMFP.ADC7_SS21_AD6=1;  
		SYS->ALTMFP.PA7_S21 =0 ;   
		SYS->ALTMFP.EBI_EN = 0;   
		
	// with unsigned format
		ADC->ADCR.DMOF = 0;  
	
	// Diffen
		ADC->ADCR.DIFFEN =0;	
	
	// ADC Operation Mode
		ADC->ADCR.ADMD = 2; 
		
	// ADC Interrupt Enable
		ADC->ADCR.ADIE =1 ;  
		
	// Analog input Channel 7 select
	ADC->ADCHER.PRESEL = 00;  
	
	// Analog input Channel 7 enable
	ADC->ADCHER.CHEN |= 0x80; 
	
	
	ADC->ADCALR.CALEN = 1; 
	while(ADC->ADCALR.CALDONE == 0);
	
	
	
	
	
	
	
}

void GPIO_INIT(void)
{
   
    //reg_ptr2=(unsigned int *)(0x50004000+0x07);
	 //*reg_ptr=0x00;
	GPIO_Open(E_GPIOC, 12, E_OUTPUT);
	GPIO_Open(E_GPIOC, 13, E_OUTPUT);
	GPIO_Open(E_GPIOC, 14, E_OUTPUT);
	GPIO_Open(E_GPIOC, 15, E_OUTPUT);
	GPIO_Open(E_GPIOA,7,E_INPUT);
    
  // Write your code here
}
void GPIO_Open(GPIO_GROUP group, unsigned int pin, GPIO_MODE mode)
{
	volatile unsigned int *reg_ptr;
	reg_ptr3 = (unsigned int *)(GPIO_BASE + group * 0x40);
	                                                              
	*reg_ptr3 |= (unsigned int)(mode << (pin << 1));   	                                                  	                                                 
}

void ADC_START(void)
{
	  
	
	ADC->ADCR.ADST=1;
	 // reg_ptr2=(unsigned int *)(0x50004000+0x07);
	 //ADC->ADDR = *reg_ptr;
	//ADC->ADCMPR[1].CMPD = (*reg_ptr2);
	//*reg_ptr2=*reg_ptr2+1;
	
	

  
 
	// Write your code here
}

void GPC_set(unsigned int pin)
{   
reg_ptr = (unsigned int *)(0x50004000 + 0x8C);
*reg_ptr = (unsigned int)(0xf<<12);
reg_ptr = (unsigned int *)(0x50004000 + 0x88);
*reg_ptr = (unsigned int)(1<<pin);
}

void seven_segment_display(unsigned char place,char number )      //, char cs, char dot)
{
	       unsigned char temp,i;
	temp=SEG_BUF[number];
	if(place==3)
	{  
	
	  temp=SEG_BUF[number]-0x02;
	
	}
		
	
	
	for(i=0;i<8;i++)
	{
		if((temp&0x01)==0x01)
		{			
		   DrvGPIO_SetBit(E_GPE,i);
		}else{
		   DrvGPIO_ClrBit(E_GPE,i);		
		}			
		   temp=temp>>1;
		GPC_set(4+place);	
	}  // Write your code here
}
void close_seven_segment(void)
{
	GPIOC->DOUT &= ~(0xF<<4);
}


void ADC_IRQHandler(void)
{
         
	static int data1,data2,data3,data4;       
	
	result=ADC->ADDR[7].RSLT;   // do not use in ADC_START
	
	if(result==0)
	
	{
		
		 DrvGPIO_SetBit(E_GPC, 12);
		DrvGPIO_SetBit(E_GPC, 13);
		DrvGPIO_SetBit(E_GPC, 14);
    DrvGPIO_SetBit(E_GPC, 15);
		SYS_Delay(4000);
	}
	
	
	 else if(result<1024 && result !=0)
	{  DrvGPIO_SetBit(E_GPC, 12);
		DrvGPIO_SetBit(E_GPC, 13);
		DrvGPIO_SetBit(E_GPC, 14);
    DrvGPIO_ClrBit(E_GPC, 15);
		SYS_Delay(4000);
	}	
	else if(result>=1024 && result<2048)
	{
		 DrvGPIO_SetBit(E_GPC, 12);
		DrvGPIO_SetBit(E_GPC, 13);
		DrvGPIO_ClrBit(E_GPC, 14);
    DrvGPIO_ClrBit(E_GPC, 15); 
		SYS_Delay(4000);
	}	
	else if(result>=2048 && result<3072) {
		 DrvGPIO_SetBit(E_GPC, 12);
		DrvGPIO_ClrBit(E_GPC, 13);
		DrvGPIO_ClrBit(E_GPC, 14);
    DrvGPIO_ClrBit(E_GPC, 15);
		SYS_Delay(4000);
	}
	else if(result>=3072)
	{
		 DrvGPIO_ClrBit(E_GPC, 12);
		DrvGPIO_ClrBit(E_GPC, 13);
		DrvGPIO_ClrBit(E_GPC, 14);
    DrvGPIO_ClrBit(E_GPC, 15);
		SYS_Delay(4000);
	}
	result=result*3.3;
	 data1=(int)(result/4096)  ;
	 data2=(int)(result/410)-data1*10;
	 data3=(int)(result/41)-data1*100-data2*10;
	data4=(int)(result/4.1)-data1*1000-data2*100-data3*10;
	seven_segment_display(3,data1);
	SYS_Delay(4000);
	seven_segment_display(2,data2);
	SYS_Delay(4000);
	seven_segment_display(1,data3);
	SYS_Delay(4000);
	seven_segment_display(0,data4);
	SYS_Delay(4000);
	      
	 close_seven_segment();
	 
	      ADC->ADSR.ADF=1;
	
	        
	
	
	     
	          // Write your code here
	       
}



int main(void)
{
	int i,j;
	double anadata;
	UNLOCKREG();
	DrvSYS_Open(50000000);// set System Clock to run at 50MHz.
	SYSCLK->CLKSEL0.STCLK_S = 0x3; // system tick's clock.
	SYSCLK->CLKSEL1.ADC_S = 2;  
	SYSCLK->CLKDIV.ADC_N = 0xE0;          //??????????????????????  
	SYSCLK->APBCLK.ADC_EN =1;  
	LOCKREG();
	GPIO_INIT();
	
	

	// ADC interrupt Set-Enable
	NVIC_EnableIRQ(29);   // ADC's GPIO
	
	ADC_INIT();
  ADC_START();
	
          // Write your code here
  
			 //us
	
	
	
}

