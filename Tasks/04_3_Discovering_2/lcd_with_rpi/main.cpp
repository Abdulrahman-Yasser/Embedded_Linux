#include "my_i2c.hpp"
#include "lcd.hpp"

#include <fcntl.h>
int main(){
    std::string the_path = "/sys/class/a7a";
    char this_is_data[5] = {0x10, 0x52, 0x21, 0x30, 0x0f};
    my_i2c this_is_my_i2c(the_path, O_RDWR);

    my_lcd::lcd the_lcd(0x27, this_is_my_i2c, my_lcd::lcd::lcd_fnc::LCD_FunctionSet_4B_2L_510_F);
    the_lcd.write_chars(the_path);
}