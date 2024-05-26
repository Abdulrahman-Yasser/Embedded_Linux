To use the lcd, you must connect it first.
You will find a file --> /dev/lcd
Make it writable --> sudo chmod 666 /dev/lcd
Write whatever you want there --> 

echo "Okei Dokie" > /dev/lcd
echo "" > /dev/lcd
