//
// Smpl_7seg_keypad
//
// Input:  3x3 keypad (input = 1~9 when key is pressed, =0 when key is not pressed
// Output: 7-segment LEDs
//
#include <stdio.h>																											 
#include "NUC1xx.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvGPIO.h"

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
#define SEG_N9   0x62


unsigned char SEG_BUF[16]={SEG_N0, SEG_N1, SEG_N2, SEG_N3, SEG_N4, SEG_N5, SEG_N6, SEG_N7, SEG_N8, SEG_N9}; 
volatile unsigned int *reg_ptr; 

void GPC_set(unsigned int pin)
{
reg_ptr = (unsigned int *)(0x50004000 + 0x8C);
*reg_ptr = (unsigned int)(0xf<<12);
reg_ptr = (unsigned int *)(0x50004000 + 0x88);
*reg_ptr = (unsigned int)(1<<pin);
}
void show_seven_segment(unsigned char place, unsigned char number)
{
  unsigned char temp,i;
	temp=SEG_BUF[number];
	
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
	}
}

void OpenKey_Pad(void)
{
	uint8_t i;
	/* Initial key pad */
	for(i=0;i<6;i++)
	DrvGPIO_Open(E_GPA, i, E_IO_QUASI);
}

uint8_t Scan_key(void)
{
	uint8_t act[3]={0x3,0x5,0x6}; // Write your code here
	uint8_t i,temp,pin;
	volatile unsigned int * ptrPin;
	ptrPin = (unsigned int *) (0x50004000 + 0x10);

	for(i=0;i<3;i++)
	{
		temp=act[i];
		for(pin=0;pin<3;pin++)
		{
			if((temp&0x01)==0x01)  
			{
				DrvGPIO_SetBit(E_GPA,pin); 
			}else{
				DrvGPIO_ClrBit(E_GPA,pin); 
			}
			temp>>=1;
		}
		
		if (((*ptrPin >>3 )& 0x1) == 0) {
       		
			   return(i+1);
		} 
		if (((*ptrPin >>4 )& 0x1) == 0) {
			return(i+4);
		}
		if (((*ptrPin >>5 )& 0x1) == 0) {
			return(i+7);
		}
	}
		return 0;
}

void add(char n1)
{
	seg[3] = seg[2];
	seg[2] = seg[1];
	seg[1] = seg[0];
	seg[0] = n1;
}

void show(void)
{
	int i;
	
	for(i = 0 ; i<4 ; i++)
	{
		show_seven_segment(i,seg[i]);		
		DrvSYS_Delay(4000);
	}
}


int32_t main (void)
{	
	int8_t number,temp_num;
	
	UNLOCKREG();
  DrvSYS_Open(48000000);
	LOCKREG();

	OpenKey_Pad();					 	
	
	while(1)
	{				
		number = Scan_key();	
		temp_num = number;	

		if(number == temp_num)
		{
			number = Scan_key();
			show();					
		}
		if(temp_num != 0)
		{    
			    add(temp_num);
          while(Scan_key()!=0) ;
						//show();			
		}
	}
	
}
