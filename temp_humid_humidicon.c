//***************************************************************************
//
// File Name            : temp_humid_humidicon.c
// Title                : temp display
// Date                 : 03/06/18
// Version              : 1.0
// Target MCU           : ATmega128 @  MHz
// Target Hardware      ; 
// Author               : Haiying Jiang &Yiwen Jin
// DESCRIPTION
// The file contains functions that make it easier for a C
// program to use the LCD display. The functiond compute_scaled_rh() 
// computes a scaled %RH from the raw 14-bit RH value extracted from reading
// the HumidIcon. The function compute_scaled_temp()computes a scaled 
// temperature from the raw 14-bit temperature value extracted from reading the
// HumidIcon. These computations must be based on the conversion formulas given
// in the HumidIcon data sheets. These functions must do all of their 
// computations using integer arithmetic.A third function named 
// meas_display_rh_temp() calls SPI_humidicon_config() to configure the 
// HumidIcon for a reading. Then calls read_humidicon() to get scaled 
// temperature and humidity values. This function then displays the results on a 
// DOG 3 x 16 LCD. An extra void read_humidicon_tsk2() is in this file.
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
#include <iom128.h>
#include <intrinsics.h>
#include <avr_macros.h>
#include <stdio.h>
#include <math.h>
extern unsigned int raw_humid;
extern int raw_temp;
extern unsigned char status;
extern unsigned char humid_H;
extern unsigned char humid_L;
extern unsigned char temp_H;
extern unsigned char temp_L;

extern void SPI_humidicon_config (void);
extern unsigned char read_humidicon_byte(void);
extern int putchar(int);

//******************************************************************************
// Function : unsigned int compute_scaled_rh(unsigned int rh)
// Date and version : version 1.0
// Target MCU : ATmega128A
// Author : Ken Short
// DESCRIPTION
// Computess scaled relative humidity in units of 0.01% RH from the raw 14-bit
// realtive humidity value from the Humidicon.
//
//
// Modified
//******************************************************************************
unsigned int compute_scaled_rh(unsigned int rh){
  unsigned int humidity =10000*(int)(rh /(0x4000-0x2));
  return humidity;
}

//******************************************************************************
// Function : unsigned int compute_scaled_temp(unsigned int temp)
// Date and version : version 1.0
// Target MCU : ATmega128A
// Author : Ken Short
// DESCRIPTION
// Computess scaled temperature in units of 0.01 degrees C from the raw 14-bit
// temperature value from the Humidicon
//
//
// Modified
//******************************************************************************
int compute_scaled_temp(unsigned int temp) {
	int tempreture;
	tempreture= 100 * (int)0xA5 * temp / (0x4000 - 0x2) - 100*0x28;
	return  tempreture;
}

//******************************************************************************
// Function : void meas_display_rh_temp()
// Date and version : version 1.0
// Target MCU : ATmega128A
// Author : Ken Short
// DESCRIPTION
//  Displays the results on a DOG 3 x 16 LCD.
// Modified
//******************************************************************************
extern void meas_display_rh_temp(void){
  SPI_humidicon_config();//initialize MISO setting
  CLEARBIT(PORTA,0);//select slave sensor
  //status=read_humidicon_byte();//MR
  humid_H=read_humidicon_byte();//FR
  humid_L=read_humidicon_byte();
  temp_H=read_humidicon_byte();
  temp_L=read_humidicon_byte();
  SETBIT(PORTA,0);//unselect slave sensor
  __delay_cycles(10000);
  humid_H &= 0x3F;//mask the higher byte of humid_H
  raw_humid=(int)humid_H *0x100 + (int)humid_L;//raw data count to int

  //unsigned temp_H_d = temp_H << 6;//left shift high byte by 6
  //temp_L = temp_L >> 2;//right shift low byte by 2
  //char temp = temp_H_d & 0xC0;//mask the lower 6 bits
  //temp_L = temp | temp_L;//combine the lower 8 bits 
  //temp_H =temp_H & 0xFC;//mask the higher byte of temp
  temp_L = temp_L >> 2;
  raw_temp = (int)temp_H * 0x40 + (int)temp_L;//raw data count to int

  int print_temp=compute_scaled_temp(raw_temp);
  unsigned int print_humid=compute_scaled_rh(raw_humid);
  int print_temp_1=(int)(print_temp/100);
  int print_temp_2=(int)(print_temp%100);
  int print_humid_1=(int)(print_humid/100);
  int print_humid_2=(int)(print_humid%100);
  printf("Module 3 Spr 18 ");
  printf("Temp:%d",print_temp_1);
  if(print_temp_2<10){
    printf(".0%d   ",print_temp_2);
  }
  else{
    printf(".%d   ",print_temp_2);
  }
 
  printf("RH:%d",print_humid_1);
  if(print_humid_2<10){
    printf(".0%d   ",print_humid_2);
  }
  else{
    printf(".%d   ",print_humid_2);
  }
  //printf('%');

}












