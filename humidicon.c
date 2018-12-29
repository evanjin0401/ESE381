#include <iom128.h>
#include <intrinsics.h>
#include <avr_macros.h>


unsigned int raw_humid;
int raw_temp;
unsigned char status;
unsigned char humid_H;
unsigned char humid_L;
unsigned char temp_H;
unsigned char temp_L;

//******************************************************************************
// Function : void SPI_humidicon_config (void)
// Date and version : version 1.0
// Target MCU : ATmega128A @ 16MHz
// Author : Haiying Jiang & Yiwen Jin
// DESCRIPTION
// This function unselects the HumidIcon and configures it for operation with
// an ATmega128A operated a 16 MHz. Pin PA0 of the ATmega128A is used to select
// the HumidIcon
//
// Modified
//******************************************************************************
extern void SPI_humidicon_config (void){
  SETBIT(PORTA,0);//unselect sensor
  //incase of conflict with int_lcd_dog(),SPR0 is also specified
  // to use SPI--As master--failing setup rising sample--fSCLK=Fosi/64
  SPCR=(1<<SPE) | (1<<MSTR) | (1<<CPOL) | (1<<CPHA) | (1<<SPR1)|(0<<SPR0);
  TESTBIT(SPSR,1<<SPIF);//clear write collision flag
  TESTBIT(SPDR,0);
 }

 
//******************************************************************************
// Function : unsigned char read_humidicon_byte(void)
// Date and version : version 1.0
// Target MCU : ATmega128A
// Author : Haiying Jiang &Yiwen Jin
// DESCRIPTION
// This function reads a data byte from the HumidIcon sensor and returns it as
// an unsigned char. The function does not return until the SPI transfer is
// completed. The function determines whether the SPI transfer is complete
// by polling the appropriate SPI status flag.
//
// Modified
//******************************************************************************
extern unsigned char read_humidicon_byte(void){  
  SPDR=0xAA;//write to the data register
  while(!(SPSR & (1<<SPIF)));//wait for transmission
  return SPDR;
}

