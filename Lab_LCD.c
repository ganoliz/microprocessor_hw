#include <math.h>
#include "NUC1xx.h"
#include <string.h>
#include <stdlib.h>
#include "Driver\DrvSYS.h"
#include "NUC1xx-LB_002\LCD_Driver.h"

void SYS_Delay(unsigned int us)
{
		static unsigned char repeat;
	
		// If sys clock is 25M Hz.
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


void Draw_pix(unsigned char x, double y, unsigned char color)
{
	
	char page=y/8;
	SetPACA(page,x);
	if(page>4.0)
	WriteData(0x01<<(char)((y/8.0- (int)page)*8));
	else
		WriteData(0x80>>(char)(8-(y/8.0-(int)page)*8));
	
  /*       if((x<=32 && x>=2) || (x>=64&&x<=96)) {                              // if(x<=16||(x>=48 && x<=80)||x>=112){
	//y=(char)y;
	SetPACA(y,x);
	WriteData(0x01<<color );
	}
	
	 else if((x>=32&&x<=64)||(x>=96)){
		
	//	y=(char)y;
	SetPACA(y,x);
	WriteData(0x80>>color );
	

	}    */
		
	
}

int main(void)
{	
	unsigned int x=300;
	int l;
	
	 double	y;
	int last_y;
	unsigned char test=1;
	char i=0;
	int j=0;
	UNLOCKREG();
	DrvSYS_Open(50000000);// set System Clock to run at 50MHz.
	SYSCLK->CLKSEL0.STCLK_S = 0x3; // Select Systemtick Source.
	LOCKREG();
	//panel initial	
	Initial_panel();
	clr_all_panel();
	
	while(1)
	{  
		for(l=2;l<129;l++)
		{ 
			 	
   
			y=20*sin(2*3.14/64*(l-x))+32;		
      Draw_pix(l,y,1);
      /* if((x<=32 && x>=2) || (x>=64&&x<=96))                                  //  if(x<=16||(x>=48 && x<=80)||x>=112)
      {
				i=(y)/(2)*16-1;
				
				if(i>7)
				{
          i=i-9;
				}					
			}
            else if((x>=32&&x<=64)||(x>=96))     //      else if((x>=16&&x<=48)||(x>=80&&x<=112))
		 {
			 i=(y)/(2)*16-1;
			 if(i>7)
			 { //y=y-1;
         i=i-9;
       }				 
			 
			 
		 }*/
		 
		 
 /*     if(last_y-y<1 && last_y-y>-1)
			{ if(test==1)
				{
				i=i+1;
				if(i>=8)
				{ y=y+1;
					i=0;
					
				 }
			 }
				else if(test==0)
				{
				 i=i-1;
					if(i<0)
					{ y=y-1;
						i=7;
						
						
					}	
		
				}
			  				 
			}
			else if(last_y-y>1)	
		{
			i=7;
			
			
		} 
			
		else	if(last_y-y<-1)
			{
				i=0;
			}
		*/	
	                
	//	 Draw_pix(x,y+4,i);
		
			//last_y=j-1;
			
			
	}
DrvSYS_Delay(500000);
	clr_all_panel();
	x=x+1;
	// Sin Function : y=Vm*sin(2*pi*f+x);
}
	

}

void HardFault_Handler(void)
{
	while(1){
		//HardFault
	}
}



