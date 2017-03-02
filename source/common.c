/*
Copyright (c) 2013-2014 Ben Croston

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Python.h"
#include "c_gpio.h"
#include "common.h"

int gpio_mode = MODE_UNKNOWN;
int setup_error = 0;
int module_setup = 0;
int revision = -1;

extern int f_a20;

//Cubietruck CN8
/*
	fex
	
[gpio_para]
gpio_used = 1
gpio_num = 21
;pin rezerved onboard hardware
gpio_pin_1 = port:PH20<1><default><default><1>
gpio_pin_2  = port:PH10<0><default><default><0>
	
;CN8 pin
gpio_pin_3 = port:PC19<1><default><default><default>
gpio_pin_4 = port:PC21<1><default><default><default>
gpio_pin_5 = port:PC20<1><default><default><default>
gpio_pin_6 = port:PC22<1><default><default><default>
gpio_pin_7 = port:PB14<1><default><default><default>
gpio_pin_8 = port:PB16<1><default><default><default>
gpio_pin_9 = port:PB15<1><default><default><default>
gpio_pin_10 = port:PB17<1><default><default><default>
gpio_pin_11 = port:PI20<1><default><default><default>
gpio_pin_12 = port:PI14<1><default><default><default>
gpio_pin_13 = port:PI21<1><default><default><default>
gpio_pin_14 = port:PI15<1><default><default><default>
gpio_pin_15 = port:PI03<1><default><default><default>
gpio_pin_16 = port:PB03<1><default><default><default>
gpio_pin_17 = port:PB02<1><default><default><default>
gpio_pin_18 = port:PB18<1><default><default><default>
gpio_pin_19 = port:PB19<1><default><default><default>
	
;pin rezerved BT hardware
gpio_pin_68 = port:PH18<0><default><default><0>
gpio_pin_69 = port:PH24<0><default><default><0>	
	
*/

const int physToGpio_BP [64] =	//BOARD MODE
{	
        -1, 		// 0
        -1, -1, 	// 1, 2
        -1, -1, 	// 3, 4
        83, 85, 	// 5, 6
        84, 86, 	// 7, 8
        46, 48, 	// 9, 10
        47, 49, 	// 11, 12
        -1, -1, 	// 13, 14
        276, 270, 	// 15, 16
        277, 271, 	// 17, 18
        259, 35, 	// 19, 20
        34, -1, 	// 21, 22
        50, -1, 	// 23, 24
        51, -1, 	// 25, 26
        -1, -1, 	// 27-> 28
        -1, -1, 	// 29-> 30
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 31-> 40
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 41-> 55
        -1, -1, -1, -1, -1, -1, -1, -1	// 56-> 63
};	

const int pinTobcm_BP [64] =	//BCM MODE
{	
        -1, 		//GPIO0
        -1, -1, 	//GPIO1,2
        83, 85, 	//GPIO3,4
        84, 86, 	//GPIO5,6
        46, 48, 	//GPIO7,8
        47, 49, 	//GPIO9,10
        276, 270, 	//GPIO11,12
        277, 271, 	//GPIO13,14
        259, 35, 	//GPIO15,16
        34, 50, 	//GPIO17,18
        51, -1, 	//GPIO19,20
        -1, -1, 	// 21-> 22
        -1, -1, 	// 23-> 24
        -1, -1, 	// 25-> 26
        -1, -1, 	// 27-> 28
        -1, -1, 	// 29-> 30
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 31-> 40
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 41-> 55
        -1, -1, -1, -1, -1, -1, -1, -1	// 56-> 63
};	

const int physToGpioR3 [64] =	//head num map to BCMpin
{	
        -1, 		// 0
        -1, -1, 	// 1, 2
        -1, -1, 	// 3, 4
        3, 4, 		// 5, 6
        5, 6, 		// 7, 8
        7, 8, 		// 9, 10
        9, 10, 		// 11, 12
        -1, -1, 	// 13, 14
        11, 12, 	// 15, 16
        13, 14, 	// 17, 18
        15, 16, 	// 19, 20
        17, -1, 	// 21, 22
        18, -1, 	// 23, 24
        19, -1, 	// 25, 26
        -1, -1, 	// 27-> 28
        -1, -1, 	// 29-> 30
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 31-> 40
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 41-> 55
        -1, -1, -1, -1, -1, -1, -1, -1	// 56-> 63
};	

int check_gpio_priv(void)
{
	// check module has been imported cleanly
	if (setup_error)
	{
		PyErr_SetString(PyExc_RuntimeError, "Module not imported correctly!");
		return 1;
	}

	// check mmap setup has worked
	if (!module_setup)
	{
		PyErr_SetString(PyExc_RuntimeError, "No access to /dev/mem.  Try running as root!");
		return 2;
	}
	return 0;
}


int is_valid_raw_port(int channel)
{
	if (channel >= 0 && channel < 18) return 1; // PA
	if (channel >= 32 && channel < 56) return 2; // PB
	if (channel >= 64 && channel < 89) return 3; // PC
	if (channel >= 96 && channel < 124) return 4; // PD
	if (channel >= 128 && channel < 140) return 5; // PE
	if (channel >= 160 && channel < 166) return 6; // PF
	if (channel >= 192 && channel < 204) return 7; // PG
	if (channel >= 224 && channel < 252) return 8; // PH
	if (channel >= 256 && channel < 278) return 9; // PI
	
	return 0;
}


int get_gpio_number(int channel, unsigned int *gpio,unsigned int *sys_gpio)
{
	// check channel number is in range
	if ( (gpio_mode == BCM && (channel < 0 || channel > 64))
	|| (gpio_mode == BOARD && (channel < 1 || channel > 30)) )
	{
		PyErr_SetString(PyExc_ValueError, "The channel sent is invalid on a Cubietruck");
		return 4;
	}

	// convert channel to gpio
	if (gpio_mode == BOARD)
	{
		if (*(*pin_to_gpio+channel) == -1)
		{
			PyErr_SetString(PyExc_ValueError, "The channel sent is invalid on a Cubietruck");
			return 5;
		} 
		else 
		{
			*gpio = *(*pin_to_gpio+channel);	//pin_to_gpio is initialized in py_gpio.c, the last several lines
			*sys_gpio = *(physToGpioR3 + channel);
		}
	}
	else if (gpio_mode == BCM)
	{
		*gpio = *(pinTobcm_BP + channel); 
		*sys_gpio = channel;
	}
	else if (gpio_mode == MODE_RAW)
	{
		if (!is_valid_raw_port(channel))
		{
			PyErr_SetString(PyExc_ValueError, "The channel sent does not exist");
			return 5;
		}
		*gpio = channel;
		
		unsigned int i;
		for (i = 0; i < 64; i++)
		{
			if (*(pinTobcm_BP + i) == channel)
			{
				*sys_gpio = i;
				break;
			}
		}
	}
	else 
	{
		// setmode() has not been run
		PyErr_SetString(PyExc_RuntimeError, "Please set pin numbering mode using GPIO.setmode(GPIO.BOARD) or GPIO.setmode(GPIO.BCM) or GPIO.setmode(GPIO.RAW)");
		return 3;
	}

	if(lemakerDebug)
		printf("GPIO = %d,sys_gpio = %d\n", *gpio,*sys_gpio);
	
	return 0;
}
