


try:
    import RPi.GPIO as GPIO
except RuntimeError:
    print("Error importing RPi.GPIO!  This is probably because you need superuser privileges.  You can achieve this by using 'sudo' to run your script")

my_channel = 20

GPIO.setmode(GPIO.BOARD)
GPIO.setup(my_channel, GPIO.OUT, initial=GPIO.HIGH)

while(True):
    sleep(1)
    GPIO.output(my_channel, GPIO.LOW)
    sleep(1)
    GPIO.output(my_channel, GPIO.HIGH)


