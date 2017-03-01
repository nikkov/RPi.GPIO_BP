RPi.GPIO_CT README

THIS CODE NOT TESTED!!!

This is a modified version RPi.GPIO for Cubietruck. We call it RPi.GPIO_CT.
It is based on modification by [LeMaker](http://lemaker.org) the original [RPi.GPIO](https://pypi.python.org/pypi/RPi.GPIO).

You can donwload the RPi.GPIO_CT from:
https://github.com/nikkov/RPi.GPIO_CT.

## Download
    git clone https://github.com/nikkov/RPi.GPIO_CT -b Cubietruck
## Installation
    sudo apt-get update
    sudo apt-get install python-dev
    cd /RPi.GPIO_CT
    python setup.py install                 
    sudo python setup.py install
    
Please be attention that you need use both python and sudo python to make the RPi.GPIO_CT work well.

## Examples
You can go to LeMaker wiki to see the basic examples: http://wiki.lemaker.org/RPi.GPIO

And the source directory test also has many demo.

## Extra
This version supports a new addressing mode "RAW" which enables you to use any GPIO pin. Below is an example which sets PG10 (which is pin 13 on the CN9 connector) to a high level.

    import RPi.GPIO as GPIO
    GPIO.setmode(GPIO.RAW)
    GPIO.setup(GPIO.PG+10, GPIO.OUT)
    GPIO.output(GPIO.PG+10, 1)


Thanks!


