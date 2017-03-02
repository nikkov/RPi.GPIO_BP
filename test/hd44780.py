# Interfacing 16x2 LCD with Raspberry Pi using GPIO & Python
# http://www.rpiblog.com/2012/11/interfacing-16x2-lcd-with-raspberry-pi.html
#!/usr/bin/python

#LCD 	Pin		Sys		CN8 pin
#----------------------------
#VSS	1		GND		1
#VCC	2	 	+V5		29
#VO		3		GND		1
#RS		4		7		9
#R/W	5		GND		1
#E		6		9		11
#DB0	7		
#DB1	8		
#DB2	9		
#DB3	10		
#DB4	11		11		15
#DB5	12		13		17
#DB6	13		15		19
#DB7	14		17		21
#LEDA	15	 	+V5		29
#LEDK	16		GND		1

import RPi.GPIO as GPIO
from time import sleep
class HD44780:

    def __init__(self, pin_rs=9, pin_e=11, pins_db=[15, 17, 19, 21]):

        self.pin_rs=pin_rs
        self.pin_e=pin_e
        self.pins_db=pins_db

        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(self.pin_e, GPIO.OUT)
        GPIO.setup(self.pin_rs, GPIO.OUT)
        for pin in self.pins_db:
            GPIO.setup(pin, GPIO.OUT)

        self.clear()

    def clear(self):
        """ Blank / Reset LCD """

        self.cmd(0x33) # $33 8-bit mode
        self.cmd(0x32) # $32 8-bit mode
        self.cmd(0x28) # $28 8-bit mode
        self.cmd(0x0C) # $0C 8-bit mode
        self.cmd(0x06) # $06 8-bit mode
        self.cmd(0x01) # $01 8-bit mode

    def cmd(self, bits, char_mode=False):
        """ Send command to LCD """

        sleep(0.001)
        bits=bin(bits)[2:].zfill(8)

        GPIO.output(self.pin_rs, char_mode)

        for pin in self.pins_db:
            GPIO.output(pin, False)

        for i in range(4):
            if bits[i] == "1":
                GPIO.output(self.pins_db[::-1][i], True)

        GPIO.output(self.pin_e, True)
        GPIO.output(self.pin_e, False)

        for pin in self.pins_db:
            GPIO.output(pin, False)

        for i in range(4,8):
            if bits[i] == "1":
                GPIO.output(self.pins_db[::-1][i-4], True)


        GPIO.output(self.pin_e, True)
        GPIO.output(self.pin_e, False)

    def message(self, text):
        """ Send string to LCD. Newline wraps to second line"""

        for char in text:
            if char == '\n':
                self.cmd(0xC0) # next line
            else:
                self.cmd(ord(char),True)

if __name__ == '__main__':

    lcd = HD44780()
    lcd.message("Cubietruck\n  Take a byte!")
	GPIO.cleanup()
	
	
	
	
#For 4x20 display, just change message function as following:	
#
#def message(self, text):
#for char in text:
#if char == '\1':
#self.cmd(0xC0) # next line
#elif char == '\2':
#self.cmd(0x94) 
#elif char == '\3':
#self.cmd(0xD4)
#else:
#self.cmd(ord(char),True)
#
#and in main while loop use:
#
#lcd.message(" "+s[:20]+"\1"+s[20:40]+"\2"+s[40:60]+"\3"+s[60:80])	