#include <iom128.h>
#include <intrinsics.h>
#include <avr_macros.h>



char dsp_buff_1[16];
char dsp_buff_2[16];
char dsp_buff_3[16];

#define SCK 1
#define MISO 3
#define MOSI 2
#define SS_bar 0
#define RS 4
#define BLC 5
#define clk_speed 16


char tmp3;

void init_spi_lcd(void);
void lcd_spi_transmit_CMD(char cmd);
void lcd_spi_transmit_DATA(char data);
extern void init_lcd_dog(void){
  char tmp;
  init_spi_lcd();
  __delay_cycles(640000); //delay for 40ms, assume 1Mhz

  tmp=0x39;                     //send function set 1
  lcd_spi_transmit_CMD(tmp);
  __delay_cycles(480);
  tmp=0x39;                     //send function set 2                
  lcd_spi_transmit_CMD(tmp);
  __delay_cycles(480);
  tmp=0x1E;                     //send bias value
  lcd_spi_transmit_CMD(tmp);
  __delay_cycles(480); 
  tmp=0x51;                     //power control
  lcd_spi_transmit_CMD(tmp);
  __delay_cycles(480); 
  tmp=0x6C;                     //follower control
  lcd_spi_transmit_CMD(tmp);
  __delay_cycles(480);
  tmp=0x71;                     //contrast set
  lcd_spi_transmit_CMD(tmp);
  __delay_cycles(480);
  tmp=0x0C;                     //display on
  lcd_spi_transmit_CMD(tmp);
  __delay_cycles(480);
  tmp=0x01;                     //clear display
  lcd_spi_transmit_CMD(tmp);
  __delay_cycles(480);
  tmp=0x06;                     //entry mode
  lcd_spi_transmit_CMD(tmp);
  __delay_cycles(480);
}

extern void update_lcd_dog(void){
  init_spi_lcd();
  char tmp;
  tmp=0x80;                     //init DDRAM add-ctr
  lcd_spi_transmit_CMD(tmp);
  __delay_cycles(480);
  for(int i=0; i<16;i++){
    char tmp=0x60;
    tmp=dsp_buff_1[i];
    lcd_spi_transmit_DATA(tmp);
    __delay_cycles(480);
  }
  
  tmp=0x90;                     //init DDRAM add-ctr
  lcd_spi_transmit_CMD(tmp);
  __delay_cycles(480);
  for(int i=0; i<16;i++){
    tmp=dsp_buff_2[i];
    lcd_spi_transmit_DATA(tmp);
    __delay_cycles(480);
  }
  
  tmp=0xA0;                     //init DDRAM add-ctr
  lcd_spi_transmit_CMD(tmp);
  __delay_cycles(480);
  for(int i=0; i<16;i++){
    tmp=dsp_buff_3[i];
    lcd_spi_transmit_DATA(tmp);
    __delay_cycles(480);
  }
  
  
}

void init_spi_lcd(void){

  SPCR=(1<<SPE) | (1<<MSTR) | (1<<CPOL) | (1<<CPHA) | (1<<SPR1) | (1<<SPR0);
  TESTBIT(SPSR,SPIF);
  TESTBIT(SPDR,0);
  
}
void lcd_spi_transmit_CMD(char cmd){
  PORTB &= ~(1<<RS); //clear bit RS
  PORTB &= ~(1<<SS_bar); //clear bit SS_bar
  SPDR=cmd;
  while(!(SPSR & (1<<SPIF))); // wait for transmission complete.
  cmd=SPDR;
  PORTB |= (1<<SS_bar); //set bit SS_bar  
}

void lcd_spi_transmit_DATA(char data){
  PORTB |= (1<<RS); //set bit RS
  PORTB &= ~(1<<SS_bar); //clear bit SS_bar
  TESTBIT(SPSR,SPIF);
  TESTBIT(SPDR,0);
  SPDR=data;
  while(!(SPSR & (1<<SPIF))); // wait for transmission complete.
  data=SPDR;
  PORTB |= (1<<SS_bar); //set bit SS_bar  
}
  
  
  
  