;**************************************************************** v1.1a
;   ATMega128 Version:                PRINT IN LANDSCAPE
;
; revised 01/30/10                     
;   
;   This AVR-asm code module is usable as an include file for assembly
;   language and or mixed asm/C application programs. The code is freely 
;   usable by any University of Stonybrook undergraduate students for any
;   and all not-for-profit system designs and or implementations.
;
;   This code is designed to be executed on an AVR ATMega micro-computer.
;   And may be readily adapted for compatibility with IAR/AVR compilers. 
;   See the IAR assembler reference guide for more information by 
;   clicking 'Help > AVR Assembly Reference Guide" on the above menus.
;
;
;*********************************************************************
;
;   This module contains procedures to initialize and update
;   DOG text based LCD display modules, including the EA DOG163M LCD
;   modules configured with three (3) 16 charactors display lines.
;
;   The display module hardware interface uses a 1-direction, write only
;   SPI interface. (See below for more information.)
;
;   The display module software interface uses three (3) 16-byte
;   data (RAM) based display buffers - One for each line of the display.
;   (See below for more information.)


;
;*********************************************************************
;
;   *** Port B Interface Definitions:
;
;
;  Port B alt names   SCK   MISO  MOSI  /SS    RS    -     -     -
;  LCD Mod Signal     D6     -     D7   /CSB   -     -     -     -
;  LCD Mod Pin #      29     -     28    38    -     -     -     -
;
;      Notes:  RS ==>  0 = command regs, 1 = data regs
;             /SS = active low SPI select signal
;
;******************

; Module must have a name for the linker
NAME lcd

;Include ATmega128 definitions
#include <iom128.h>

; All functions used esternally must be declared public
PUBLIC init_lcd_dog
PUBLIC update_lcd_dog

PUBLIC dsp_buff_1
PUBLIC dsp_buff_2
PUBLIC dsp_buff_3

#define	SCK     1
#define	MISO    3
#define	MOSI    2
#define	SS_bar  0
#define	RS      4
#define	BLC     5	

#define	clk_speed = 16	; clock speed in MHz

;***  DATA Segment  **************************************************
;Puts display buffers in near initialize to zero segment.
;See chapter on segments in IAR reference guide
RSEG NEAR_Z
dsp_buff_1   DS8 16

dsp_buff_2   DS8 16

dsp_buff_3   DS8 16




;***  CODE Segment Subroutines  ************************************
;Puts object code in CODE segment.
RSEG CODE

;************************
;NAME:        delay_30uS
;ASSUMES:     nothing
;RETURNS:     nothing
;MODIFIES:    R24, SREG
;CALLED BY:   init_dsp
;DESCRIPTION: This procedure will generate a fixed delay of just over
;             30 uS (assuming a 1 MHz clock).
;********************************************************************
delay_30uS:   nop     ; fine tune delay
              nop
              push  r24
              ldi   r24, 255 ; load delay count.
d30_loop:     dec   r24        ; count down to
              brne  d30_loop   ; zero.
              pop   r24
              ret



;*********************
;NAME:        v_delay
;ASSUMES:     R22, R23 = initial count values defining how many
;             30uS delays will be called. This procedure can generate
;             short delays (r23 = small #) or much longer delays (where
;             R23 value is large).
;RETURNS:     nothing
;MODIFIES:    R22, R23, SREG
;CALLED BY:   init_dsp, plus...
;DESCRIPTION: This procedure will generate a variable delay for a fixed
;             period of time based the values pasted in R24 and R25.
;
;Sample Delays:
;
;             R22  R23  DelayTime
;             ---  ---  ---------
;              1    1   ~65.5 uS
;              0    1   ~14.2 mS
;              0    9   ~130 mS


;**********************************************************************
v_delay:      rcall delay_30uS ; delay for ~30uS
              dec   r22      ; decrement inner loop value, and
              brne  v_delay  ; loop until zero.
              dec   r23      ; decr outer loop count, and loop back
              brne  v_delay  ; to inner loop delay until r23 zero.
              ret




;***********************
;NAME:        delay_40mS
;ASSUMES:     nothing
;RETURNS:     nothing
;MODIFIES:    R22,R23, SREG
;CALLED BY:   init_dsp, ????
;DESCRIPTION: This procedure will generate a fixed delay of just over
;             40 mS.
;********************************************************************
delay_40mS:   ldi   r22,0        ; load inner loop var
              ldi   r23,4        ; load outer loop var
              rcall  v_delay     ; delay
              ret




;************************
;NAME:       init_spi_lcd
;ASSUMES:    IMPORTANT: PortB set as output (during program init)
;RETURNS:    nothing
;MODIFIES:   DDRB, SPCR
;CALLED BY:  init_dsp, update
;DESCRITION: init SPI port for command and data writes to LCD via SPI
;********************************************************************
init_spi_lcd:
      push r16
      ldi r16,(1<<SPE) | (1<<MSTR) | (1<<CPOL) | (1<<CPHA) | (1<<SPR1) | (1<<SPR0)
      out SPCR,r16   ; Enable SPI, Master, fck/64, 
      
      ;kill any spurious data...
      in r16, SPSR   ; clear SPIF bit in SPSR
      in r16, SPDR   ;
      pop r16        ; restore r16 value...
      ret



;********************************
;NAME:       lcd_spi_transmit_CMD
;ASSUMES:    r16 = byte for LCD.
;            SPI port is configured.
;RETURNS:    nothing
;MODIFIES:   R16, PortB, SPCR
;CALLED BY:  init_dsp, update
;DESCRITION: outputs a byte passed in r16 via SPI port. Waits for data
;            to be written by spi port before continuing.
;*********************************************************************
lcd_spi_transmit_CMD:
     push r16          ; save command, need r16.
     cbi   PORTB, RS    ; RS = 0 = command.
     cbi   PORTB, SS_bar  ;/SS = selected.
     pop r16           ; restore command
     out SPDR,r16      ; write data to SPI port.


     ;Wait for transmission complete
wait_transmit:
     in r16, SPSR     ; read status reg
     sbrs r16, SPIF   ; if bit 7  = 0 wait
     rjmp wait_transmit

	; clear SPIF bit in SPSR
     in r16, SPDR   ; read of SPSR followed by read of SPDR clears SPIF 013110 kls

     sbi   PORTB, SS_bar   ; /SS = deselected
     ret




;*********************************
;NAME:       lcd_spi_transmit_DATA
;ASSUMES:    r16 = byte to transmit to LCD.
;            SPI port is configured.
;RETURNS:    nothing
;MODIFIES:   R16, SPCR
;CALLED BY:  init_dsp, update
;DESCRITION: outputs a byte passed in r16 via SPI port. Waits for
;            data to be written by spi port before continuing.
;*****************************************************************
lcd_spi_transmit_DATA:
     push r16          ; save command, need r16.
     sbi   PORTB, RS    ; RS = 1 = data.
     cbi   PORTB, SS_bar  ; /SS = selected.
        in r16, SPSR   ; clear SPIF bit in SPSR.
        in r16, SPDR   ;
     pop r16           ; restore command.
     out SPDR,r16      ; write data to SPI port.


     ;Wait for transmission complete
wait_transmit1:
     in r16, SPSR     ; read status reg
;     in r17, SPDR     ; and clear SPIF	 ** error if in, clears SPIF early
     sbrs r16, SPIF   ; if bit 7  = 0 wait
     rjmp wait_transmit1

	; clear SPIF bit in SPSR
     in r16, SPDR   ; read of SPSR followed by read of SPDR clears SPIF 013110 kls

     sbi   PORTB, SS_bar   ; /SS = deselected
     ret



;************************
;NAME:       init_lcd_dog
;ASSUMES:    nothing
;RETURNS:    nothing
;MODIFIES:   R16, R17
;CALLED BY:  main application
;DESCRITION: inits DOG module LCD display for SPI (serial) operation.
;NOTE:  Can be used as is with MCU clock speeds of 4MHz or less.
;********************************************************************
; public __version_1 void init_dsp(void)
init_lcd_dog:
       rcall init_spi_lcd   ; init SPI port for DOG LCD.


start_dly_40ms:
       rcall  delay_40mS    ; startup delay.


func_set1:
       ldi    r16,0x39      ; send fuction set #1
       rcall  lcd_spi_transmit_CMD   ;
       rcall  delay_30uS    ; delay for command to be processed


func_set2:
       ldi    r16,0x39      ; send fuction set #2
       rcall  lcd_spi_transmit_CMD
       rcall  delay_30uS    ; delay for command to be processed


bias_set:
       ldi    r16,0x1E      ; set bias value.
       rcall  lcd_spi_transmit_CMD
       rcall  delay_30uS    ;


power_ctrl:
       ldi    r16,0x50      ; 0x50 nominal (delicate adjustment). 
       rcall  lcd_spi_transmit_CMD
       rcall  delay_30uS    ;


follower_ctrl:
       ldi    r16,0x6C      ; follower mode on... 
       rcall  lcd_spi_transmit_CMD
       rcall  delay_40mS    ;


contrast_set:
       ldi    r16,0x77      ; 7C was too bright  
       rcall  lcd_spi_transmit_CMD
       rcall  delay_30uS    ; 


display_on:
       ldi    r16,0x0c      ; display on, cursor off, blink off
       rcall  lcd_spi_transmit_CMD
       rcall  delay_30uS    ; 


clr_display:
       ldi    r16,0x01      ; clear display, cursor home
       rcall  lcd_spi_transmit_CMD


       rcall  delay_30uS    ;


entry_mode:
       ldi    r16,0x06      ; clear display, cursor home
       rcall  lcd_spi_transmit_CMD;
       rcall  delay_30uS    ;
       ret




;**************************
;NAME:       update_lcd_dog
;ASSUMES:    display buffers loaded with display data
;RETURNS:    nothing
;MODIFIES:   R16,R20,R30,R31,SREG
;
;DESCRITION: Updates the LCD display lines 1, 2, and 3, using the
;  contents of dsp_buff_1, dsp_buff_2, and dsp_buff_3, respectively.
;*******************************************************************
; public __version_1 void update_dsp_dog (void)
update_lcd_dog:
       rcall init_spi_lcd    ; init SPI port for LCD.
       ldi   r20,16          ; init 'chars per line' counter.
       push  r20             ; save for later used.


       ;send line 1 to the LCD module.
wr_line1:     
       ldi   ZH, high (dsp_buff_1)  ; init ptr to line 1 display buffer.
       ldi   ZL, low (dsp_buff_1)   ;
snd_ddram_addr:
       ldi   r16,0x80             ; init DDRAM addr-ctr 
       rcall lcd_spi_transmit_CMD ; 
       rcall delay_30uS
snd_buff_1:
       ld    r16, Z+
       rcall lcd_spi_transmit_DATA
       rcall delay_30uS
       dec   r20
       brne  snd_buff_1


       ;send line 2 to the LCD module.
init_for_buff_2:
       pop   r20     ; reload r20 = chars per line counter
       push  r20     ; save for line 3
wr_line2:
       ldi   ZH, high (dsp_buff_2)  ; init ptr to line 2 display buffer.
       ldi   ZL, low (dsp_buff_2)
snd_ddram_addr2:
       ldi   r16,0x90               ; init DDRAM addr-ctr 
       rcall lcd_spi_transmit_CMD           ;  
       rcall delay_30uS
snd_buff_2:
       ld    r16, Z+
       rcall lcd_spi_transmit_DATA
       rcall delay_30uS
       dec   r20
       brne  snd_buff_2


       ;send line 3 to the LCD module.
init_for_buff_3:
       pop   r20     ; reload r20 = chars per line counter
wr_line3:
       ldi   ZH, high (dsp_buff_3)  ; init ptr to line 2 display buffer.
       ldi   ZL, low (dsp_buff_3)
snd_ddram_addr3:
       ldi   r16,0xA0               ; init DDRAM addr-ctr 
       rcall lcd_spi_transmit_CMD           ;  
       rcall delay_30uS


snd_buff_3:
       ld    r16, Z+
       rcall lcd_spi_transmit_DATA
       rcall delay_30uS
       dec   r20
       brne  snd_buff_3
       ret

;******* End Of Code Segment *********************************
;******* End Of Module *********************************
END