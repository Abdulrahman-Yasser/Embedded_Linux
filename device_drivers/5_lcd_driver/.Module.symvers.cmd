cmd_/home/abdu/Study/Embedded_Linux/device_drivers/5_lcd_driver/Module.symvers := sed 's/\.ko$$/\.o/' /home/abdu/Study/Embedded_Linux/device_drivers/5_lcd_driver/modules.order | scripts/mod/modpost -m -a  -o /home/abdu/Study/Embedded_Linux/device_drivers/5_lcd_driver/Module.symvers -e -i Module.symvers   -T -
