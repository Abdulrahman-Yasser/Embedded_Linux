 
while(1):
    try:
        file1 = open("/sys/bus/iio/devices/iio:device0/in_temp_input","r")
        break
    except:
        file1.close()
        pass    
while(1):
    try:
        print(file1.readlines())
        break
    except:
        pass
print()
file1.close()

