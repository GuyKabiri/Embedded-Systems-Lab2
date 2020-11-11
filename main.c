#include <xc.h>
#include "main.h"
void main()
{
    init();
   // char control[]={0x38,0x38,0x38,0xe,0x6,0x1,0x80,0xc0};
    while(1)
    {
        reset_LEDs();
        
        if (PORTFbits.RF4)          //  if SW2 is ON
            fan();
        else if (PORTFbits.RF5)     //  if SW1 is ON
           shift();
        else if(PORTFbits.RF3)      //  if SW0 is ON
            counter();
        
        if(PORTBbits.RB9)           //  if SW7 is ON
            break;                  //  break from the loop and end program
        
        make_sound();
    }
    
    reset_LEDs();                   //  turn OFF the LEDs
}
void screenWrite(char* str, int upOrDown)
{
    
    int i;
    int control = upOrDown ? 0xc0: 0x80;
    TRISBbits.TRISB15 = 0; // RB15 (DISP_RS) set as an output
    ANSELBbits.ANSB15 = 0; // disable analog functionality on RB15 (DISP_RS)
    TRISDbits.TRISD5 = 0; // RD5 (DISP_RW) set as an output
    TRISDbits.TRISD4 = 0; // RD4 (DISP_EN) set as an output
    //TRISEbits.TRISE0 = 1; // RE0 (DB0) set as input (change 1 to 0 for
    TRISE&=0xff00;
    PORTBbits.RB15=0;//rs=0
    PORTDbits.RD5=0;//w=0
    ANSELEbits.ANSE7 = 0;

    PORTE=control;
    PORTDbits.RD4=1;
    PORTDbits.RD4=0;
    //for(j=0;j<32000;j++);
    busy();
    
    PORTBbits.RB15=1;   //rs=1
    PORTDbits.RD5=0;    //w=0
    for(i=0;i<16;i++)
    {
        PORTE=str[i];
        PORTDbits.RD4=1;
        PORTDbits.RD4=0;
        //for(j=0;j<32000;j++);
        busy();
    }

}
void busy(void)
{
    char RD,RS;
    int STATUS_TRISE;
    int portMap;
    RD=PORTDbits.RD5;
    RS=PORTBbits.RB15;
    STATUS_TRISE=TRISE;
	PORTDbits.RD5 = 1;//w/r
	PORTBbits.RB15 = 0;//rs 
               portMap = TRISE;
	portMap |= 0x80;
	TRISE = portMap;
do
     {
     PORTDbits.RD4=1;//enable=1
     PORTDbits.RD4=0;//enable=0
     }
   while(PORTEbits.RE7);// BF ?????
        PORTDbits.RD5=RD; 
        PORTBbits.RB15=RS;
        TRISE=STATUS_TRISE;   
}

void turnOnScreen(int mode)
{
    if(mode == 0)
    {
        screenWrite("Mode 0:         ", 0);
         if (PORTDbits.RD15) // sw3 is ON
         {
             if (PORTDbits.RD14) // sw3 & sw4 is ON
                 screenWrite("Count Down Fast ",1);
             else               // sw3 is ON & sw4 is OFF
                 screenWrite("Count Down Slow ",1);
         }
         else if(PORTDbits.RD14) // sw3 is OFF & sw4 is ON
                screenWrite("Count Up Fast   ",1);
         else                   // sw3 is OFF & sw4 is OFF   
                screenWrite("Count Up Slow   ",1);
    }
    
    if(mode == 1)
    {
         screenWrite("Mode 1:         ", 0);
         if (PORTDbits.RD15) // sw3 is ON
         {
             if (PORTDbits.RD14) // sw3 & sw4 is ON
                 screenWrite("Shift Right Fast",1);
             else               // sw3 is ON & sw4 is OFF
                 screenWrite("Shift Right Slow",1);
         }
         else if(PORTDbits.RD14) // sw3 is OFF & sw4 is ON
                screenWrite("Shift Left Fast ",1);
         else                   // sw3 is OFF & sw4 is OFF   
                screenWrite("Shift Left Slow ",1);
    }
        
        if(mode == 2)
    {
         screenWrite("Mode 2:         ", 0);
         if (PORTDbits.RD15) // sw3 is ON
         {
             if (PORTDbits.RD14) // sw3 & sw4 is ON
                 screenWrite("Swing Down Fast ",1);
             else               // sw3 is ON & sw4 is OFF
                 screenWrite("Swing Down Slow ",1);
         }
         else if(PORTDbits.RD14) // sw3 is OFF & sw4 is ON
                screenWrite("Swing Up Fast   ",1);
         else                   // sw3 is OFF & sw4 is OFF   
                screenWrite("Swing Up Slow   ",1);
    }
}

void counter()
{
    //  while SW0 is ON and both SW1 and SW2 are OFF (if SW1 or SW2 is ON -> break from this function)
    //  if SW7 is ON -> BREAK
    while(PORTFbits.RF3 && !PORTFbits.RF5 && !PORTFbits.RF4 && !PORTBbits.RB9)
    {
        if (PORTDbits.RD15)     //  if SW3 is ON
            PORTA = (PORTA -1) & 0xff;        //  count down
        else
            PORTA = (PORTA + 1) & 0xff;       //  otherwise, count up
        
        turnOnScreen(0);
        delay();                //  delay between each iteration
        make_sound();
    }
}


void shift()
{
    //  if SW1 is ON and SW2 is OFF (if SW2 is ON -> break from this function)
    //  if SW7 is ON -> BREAK
    while (PORTFbits.RF5 && !PORTFbits.RF4 && !PORTBbits.RB9)
    {
        if ((0xff & PORTA) == 0x00) //  if all the LEDs is OFF -> need to initiate them
       {
           if (PORTDbits.RD15)      //  if SW3 is turned ON
                PORTA = 0x80;       //  initiate with LED7 ON
           else
                PORTA = 0x1;        //  otherwise, initiate with LED0 ON
       }
        turnOnScreen(1);
        delay();                    //  delay between each iteration
        make_sound();               
        if (PORTDbits.RD15)         //  if SW3 is ON
            PORTA >>= 1;            //  shift the LEDs to the RIGHT
        else
            PORTA <<= 1;            //  otherwise, shift to the LEFT
        
    }
}

//      LEDs: 0010 0100                         LEDSs:  0010 0100                         
//      AND   1111 0000                         AND     0000 1111
//      RES:  0010 0000 shift << 1              RES:    0000 0100   shift >> 1
//            0100 0000            <-OR->               0000 0010
//                FINAL RESULT:   0100 0010
void fan()
{
//  while SW2 is ON
//  if SW7 is ON -> BREAK
    while(PORTFbits.RF4 && !PORTBbits.RB9)
    {
        if ((0xff & PORTA) == 0x0)  //  if all the LEDs is OFF -> need to initiate them
        {
            if (PORTDbits.RD15)     //  if SW3 is turned ON
                PORTA = 0X81;       //  initiate LEDs with  1000 0001
            else
                PORTA = 0x18;       //  otherwise, init as  0001 1000
        }  
        turnOnScreen(2);
        delay();                    //  delay between each iteration
        make_sound();
        
        if (PORTDbits.RD15)         //  if SW3 is ON, shift the LEDs towards INSIDE
        {   //  the 2nd & operator is to make sure only the 4 LEDs was shifted in each side
            PORTA = (((PORTA & 0xf0) >> 1) & 0xf0) | (((PORTA & 0x0f) << 1) & 0x0f);
        }
        else                        //  otherwise, shift the LEDs towards the OUTSIDE
            PORTA = ((PORTA & 0xf0) << 1) | ((PORTA & 0x0f) >> 1); 
    }
}

void make_sound()
{
    //  if SW6 is OFF -> exit the function and do not make sound
    if (!PORTBbits.RB10) //SW6
        return;
    
    int i, num = 0; 
    while(num++ < 10)
    {
        PORTBbits.RB14--;
        for(i = 0; i < 500 ; i++)   // delay between the two beeps
            ;
        PORTBbits.RB14++;
        for(i = 0; i < 500; i++)   // delay between the two beeps
            ;
    }
    
    //  if neither SW0, SW1 nor SW2 is ON, add delay
    if (!(PORTFbits.RF3 || PORTFbits.RF5 || PORTFbits.RF4))
        delay();
        
}

void delay()
{
    int interval = 80000;
    if (!PORTDbits.RD14)    //  if SW4 is OFF slower the speed
        interval = 120000;
    
    int j;
    for (j = 0; j < interval; j++)  //  the actual delay
        ;
    
    while (PORTBbits.RB11)  //  while SW5 is ON, pause the iteration
        ;
}

void reset_LEDs()
{
    PORTA = 0x0;
}

void reset_LCDs()
{
   
}

void init()
{
    TRISA &= 0xff00;        //  configure all the LEDs as output
     
    TRISFbits.TRISF3 = 1; // RF3 (SW0) configured as input
    TRISFbits.TRISF5 = 1; // RF5 (SW1) configured as input
    TRISFbits.TRISF4 = 1; // RF4 (SW2) configured as input
    TRISDbits.TRISD15 = 1; // RD15 (SW3) configured as input
    TRISDbits.TRISD14 = 1; // RD14 (SW4) configured as input
    TRISBbits.TRISB11 = 1; // RB11 (SW5) configured as input
    ANSELBbits.ANSB11 = 0; // RB11 (SW5) disabled analog
    TRISBbits.TRISB10 = 1; // RB10 (SW6) configured as input
    ANSELBbits.ANSB10 = 0; // RB10 (SW6) disabled analog
    TRISBbits.TRISB9 = 1; // RB9 (SW7) configured as input
    ANSELBbits.ANSB9 = 0; // RB9 (SW7) disabled analog
    TRISBbits.TRISB14 = 0; //speaker configured output
    ANSELBbits.ANSB14 = 0; //speaker disabled analog
    
    TRISBbits.TRISB15 = 0; // RB15 (DISP_RS) set as an output
    ANSELBbits.ANSB15 = 0; // disable analog functionality on RB15 (DISP_RS)
    TRISDbits.TRISD5 = 0; // RD5 (DISP_RW) set as an output
    TRISDbits.TRISD4 = 0; // RD4 (DISP_EN) set as an output
    //TRISEbits.TRISE0 = 1; // RE0 (DB0) set as input (change 1 to 0 for
    TRISE&=0xff00;
    ANSELEbits.ANSE2 = 0;
    ANSELEbits.ANSE4 = 0;
    ANSELEbits.ANSE5 = 0;
    ANSELEbits.ANSE6 = 0;
    PORTBbits.RB15=0;//rs=0
    PORTDbits.RD5=0;//w=0
    ANSELEbits.ANSE7 = 0;
}
