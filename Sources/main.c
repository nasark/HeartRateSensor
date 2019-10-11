//*********************************************************************                  
//*                          Final Project                            *
//*                       McMaster University                         *
//*                      2DP4 Microcontrollers                        *
//*                          Lab Section 02                           *
//*                  Nasar Khan khann16 400067997                     *
//*                                                                   *
//*********************************************************************

#include <hidef.h>      
#include "derivative.h"  
#include "SCI.h"
  
int mode = 0, beat=0, i=0, bpm, threshold = 0, trigger=0;  //initialization on global variables
unsigned short sample, sample2=0;
   
void setClk(void);                           //function prototypes
void delay1ms (unsigned int multiple);
void BCD (int bpm);

void main(void) {
  
     
  
  setClk();         // set E-Clock for 12MHz
  ATDCTL1 = 0x4F;		// set for 12-bit resolution
	ATDCTL3 = 0x88;		// right justified, one sample per sequence
	ATDCTL4 = 0x02;		// prescaler = 2; ATD clock = 12MHz / (2 * (2 + 1)) == 2MHz
	ATDCTL5 = 0x24;		// continuous conversion on channel 4
  
  
  DDRJ = 0xFF;      
  ATDDIEN = 0x0F;   //initialization of ports for BCD display
  DDR1AD = 0x0F;    //PortAD[0:3] are configured for output - least significant digit of BPM
  DDRP = 0x0F;      //PortP[0:3] are configured for output - middle digit of BPM
  DDRM = 0x01;      //PortM[0] is configured for output - most significant digit of BPM

 // configure timer input registers for push button interrupt                                                  
            
  TSCR1 = 0x90;    //enables timer and configures such that read/write clears interrupt flags

  TSCR2 = 0x01;    //timer clock set for E-clock/1 = 12MHz
                    
  TIOS = 0xFE;     //set TIC[0] and input capture
                    
  PERT = 0x01;     //enable a pull-up resistor on TIC[0]

  TCTL3 = 0x00;    //configure which edge to capture
  TCTL4 = 0x02;    //configure for falling edge on TIC[0]
  
  TIE = 0x01;      //configure timer interrupt enable


  EnableInterrupts;
  SCI_Init(57600);        //initialize baud rate of 57600 for serial communication
  
  threshold = 2048;//threshold voltage in digital calculated by:(2.5/5)*(2^12-1) where VRH is 5V and VRL is 0V. - used as a reference point in signal to determine if a beat has occured
  
  for(;;) {
    beat=1;
    bpm=0;
    sample=0;
    
    if (mode==1) {         //if button is "on", serial communication intiiates
    
      for(i=0;i<200;i++){  //200 iterations where each iteration takes 50ms - therefore for-loop execution time is 200*0.05 = 10s and so signal is sampled for 10 seconds
        
        sample=ATDDR0;      //retrieve digital sample of signal
      
        SCI_OutUDec(sample); //sample is outputted to matlab       
        delay1ms(50);     //since sampling frequency is 20Hz, therefore 1/20 = 50ms delay
      
        sample2=ATDDR0;   //another sample value is retrieved after delay
      
        if(sample>threshold && sample2<threshold){ //if first sample is greater than threshold voltage and sample after delay is lower than the threshold voltage, this means that the peak of the signal (amplitude) has passed and therefore a beat has occured
        beat++;                                    //number of beats is accumulated
        }
      
        SCI_OutChar(CR) ;           //terminator
     }

     
     bpm = beat*6; //(# of beats/10 seconds)*60 seconds per minute = BPM  //calculation is simplified in order to avoid integer division
     BCD(bpm);     //output BPM in BCD to 9 LEDs
      
     }   
     else{                //if button is "off", program remains idle and as a result serial communication stops
      
       }

  } 
  
}
interrupt  VectorNumber_Vtimch0 void ISR_Vtimch0(void)         //interrupt used for push button to control serial communication
{
  
  unsigned int temp;
  
  delay1ms(200);                   //0.2s delay in order to avoid debouncing

  if (mode == 0){                  //flips state of mode when button is pressed - mode=1 button is "on" mode=0 button is "off"
    mode=1;     
    PTJ = 0x01;                    //ESDX onboard LED is turned on to indicate state of button/serial communication
  } else{
    mode=0; 
    PTJ = 0x00;
  }
  

  temp = TC0;       // allows for another TIC interrupt
  }



void setClk(void){             //sets ESDX E-clock to 12MHz using the oscillator
 
 CPMUREFDIV = 0x80;          //REFDIV = 0 and since 6MHz<fref<=12MHz, REFFRQ[1:0] = 10                                                                 
 CPMUSYNR = 0x02;            //SYNDIV = 2 therefore: fvco = 2*fref*(SYNDIV+1) = 2*8MHz*(2+1) = 48MHz, since fvco is in the range 32MHz<=fvco<=48MHz, VCOFRQ[1:0] = 00
 CPMUPOSTDIV = 0x01;         //POSTDIV = 1 therefore since PLL is locked: fpll = fvco/(POSTDIV+1) = 48MHz/(1+1) = 24MHz
 CPMUCLKS = 0x80;            //PLLSEL = 1 therefore E-clock = fpll/2 = 24MHz/2 = 12MHz
 CPMUOSC = 0x80;             //oscillator enabled therefore: fref = fosc/(REDDIV+1) = 8/(0+1) = 8MHz
 
 while(!(CPMUFLG & 0x08));   //LOCK=1 - writing to the LOCK bit in this register has no effect, hence the bit operator
}


void delay1ms(unsigned int multiple){   //delay function implemented from lab 4 sample code from avenue and adapted for 12MHz E clock
  unsigned int i;
  unsigned int j;
  
  for(j = 0; j<multiple; j++){
    for(i = 0; i<131; i++){     //delay is created by wasting time with futile commands
      PTJ = PTJ;
      PTJ = PTJ;
      PTJ = PTJ;
      PTJ = PTJ;
      PTJ = PTJ;
    }
  }   
}

void BCD (int bpm){              //converts BPM to BCD and outputs each digit to respective port
  int lsd, sd, msd=0, leftover;   //if BPM is 2 digits long, by default the most significant digit (3rd digit) is set to 0
  
   lsd = bpm%10;             //least significant digit is calculated by mod10
   leftover = bpm/10;        //leftover digits in BPM is simply the result of integer division with 10
   
   sd = leftover%10;         //process is repeated for second digit
   leftover = leftover/10;
   
   if (leftover!=0){        //if there are leftover digits after the second digit, this means BPM is 3 digits long and so the most significant digit is equal to the leftover digit
    msd = leftover;
   }
   
   PT1AD = lsd;            //digits are outputted to ports
   PTP = sd;
   PTM = msd; 
   
   
}
