#!/bin/bash
gpioset gpiochip2 0=0 1=0 2=0 3=0 4=0 5=0 6=0 7=0
sleep 1
gpioset gpiochip2 0=0 1=0 2=0 3=0 4=0 5=0 6=0 7=1
sleep 1
gpioset gpiochip2 0=0 1=0 2=0 3=0 4=0 5=0 6=1 7=1
sleep 1
gpioset gpiochip2 0=0 1=0 2=0 3=0 4=0 5=1 6=1 7=1
sleep 1
gpioset gpiochip2 0=0 1=0 2=0 3=0 4=1 5=1 6=1 7=1
sleep 1
gpioset gpiochip2 0=0 1=0 2=0 3=1 4=1 5=1 6=1 7=1
sleep 1
gpioset gpiochip2 0=0 1=0 2=1 3=1 4=1 5=1 6=1 7=1
sleep 1
gpioset gpiochip2 0=0 1=1 2=1 3=1 4=1 5=1 6=1 7=1
sleep 1
gpioset gpiochip2 0=1 1=1 2=1 3=1 4=1 5=1 6=1 7=1
sleep 1


