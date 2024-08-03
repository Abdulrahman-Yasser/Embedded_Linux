#!/bin/bash
gpioset gpiochip0 20=1
sleep(1)
gpioset gpiochip0 20=0
sleep(1)
gpioinfo
