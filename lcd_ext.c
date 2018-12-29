//***************************************************************************
//
// File Name            : lcd_ext.c
// Title                : LCD Utilities
// Date                 : 02/07/10
// Version              : 1.0
// Target MCU           : ATmega128 @  MHz
// Target Hardware      ; 
// Author               : Ken Short
// DESCRIPTION
// The file contains two functions that make it easier for a C
// program to use the LCD display. The function clear_dsp() clears the display
// buffer arrays. When followed by the function update_dsp(), the 
// display is blanked.
//              
// The function putchar() puts a single character, passed to it as an argument,
// into the display buffer at the position corresponding to the value of
// variable index. This putchar function replaces the standard putchar funtion,
// so a printf statement will print to the LCD   
//
// Warnings             : none
// Restrictions         : none
// Algorithms           : none
// References           : none
//
// Revision History     : Initial version 
// 
//
//**************************************************************************

extern char dsp_buff_1[16];
extern char dsp_buff_2[16];
extern char dsp_buff_3[16];

static char index;    // index into display buffer

//******************************************************************************
// Function             : void clear_dsp(void)
// Date and version     : 02/07/10, version 1.0
// Target MCU           : ATmega128
// Author               : Ken Short
// DESCRIPTION
// Clears the display buffer. Treats each 16 character array separately.
// NOTE: update_dsp must be called after to see results
//
// Modified 
//******************************************************************************

void clear_dsp(void)
{
  // assuming buffers might not be contiguous
  for(char i = 0; i < 16; i++)
    dsp_buff_1[i] = ' ';
  
  for(char i = 0; i < 16; i++)
    dsp_buff_2[i] = ' ';
  
  for(char i = 0; i < 16; i++)
    dsp_buff_3[i] = ' ';  
  
  index = 0;
}


//******************************************************************************
// Function             : int putchar(int c)
// Date and version     : 02/07/10, version 1.0
// Target MCU           : ATmega128
// Author               : Ken Short
// DESCRIPTION
// This function displays a single ascii chararacter c on the lcd at the
// position specifieb by the global variable index
// NOTE: update_dsp must be called after to see results
// 
//
// Modified 
//******************************************************************************

int putchar(int c)

{
  if((char)c == '\n'){
    if(index<16)
      index=16;
    else if (index<32)
      index=32;
    else if (index<48)
      index=0;
    else 
      index=16;
  }
  else if ((char)c=='\b')
    index--;
  else if ((char)c=='\f')
    index=0;
  else if ((char)c=='\r'){
    if(index<16)
      index=0;
    else if(index<32)
      index=16;
    else if (index<48)
      index=32;
    else
      index=0;
  }
  else {
    if (index < 16) 
      dsp_buff_1[index++] = (char)c;
  
    else if (index < 32)
      dsp_buff_2[index++ - 16] = (char)c;
  
    else if (index < 48)
      dsp_buff_3[index++ - 32] = (char)c;
  
    else
      {index = 0;
       dsp_buff_1[index++] = (char)c;
      }
  } 
  
  return c;}
