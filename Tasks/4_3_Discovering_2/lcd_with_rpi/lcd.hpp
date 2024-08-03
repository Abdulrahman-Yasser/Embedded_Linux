#ifndef LCD_HPP
#define LCD_HPP

#include "my_i2c.hpp"
#include <string>

#define INIT_MSG "Hey, your lcd is initialized"

namespace my_lcd{
class lcd{
public:
    enum lcd_fnc{
        LCD_FunctionSet_4B_1L_57_F, LCD_FunctionSet_4B_1L_510_F,
        LCD_FunctionSet_4B_2L_57_F, LCD_FunctionSet_4B_2L_510_F,
        LCD_FunctionSet_8B_1L_57_F, LCD_FunctionSet_8B_1L_510_F,
        LCD_FunctionSet_8B_2L_57_F, LCD_FunctionSet_8B_2L_510_F,
    };

    enum lcd_commands{
        LCD_I2C_INST_CLEAR = 1,
        LCD_I2C_INST_CURSOR_TO_HOME = 2,
        LCD_I2C_INST_CURSOR_DEC_LEFT = 4,
        LCD_I2C_INST_DISPLAY_SHIFT_RIGHT = 5,
        LCD_I2C_INST_CURSOR_INC_RIGHT = 6,
        LCD_I2C_INST_DISPLAY_SHIFT_LEFT = 7,
        LCD_I2C_INST_DISPLAY_OFF_CURSOR_OFF_BLK_OFF = 8,
        LCD_I2C_INST_DISPLAY_OFF_CURSOR_OFF_BLK_ON,
        LCD_I2C_INST_DISPLAY_OFF_CURSOR_ON_BLK_OFF,
        LCD_I2C_INST_DISPLAY_OFF_CURSOR_ON_BLK_ON,
        LCD_I2C_INST_DISPLAY_ON_CURSOR_OFF_BLK_OFF,
        LCD_I2C_INST_DISPLAY_ON_CURSOR_OFF_BLK_ON,
        LCD_I2C_INST_DISPLAY_ON_CURSOR_ON_BLK_OFF,
        LCD_I2C_INST_DISPLAY_ON_CURSOR_BLK_ON,
        LCD_I2C_INST_CURSOR_SHIFT_LEFT = 0x28,
        LCD_I2C_INST_CURSOR_SHIFT_RIGHT = 0x38,
        LCD_I2C_INST_CURSOR_GO_1ST_LINE = 0x80,
        LCD_I2C_INST_CURSOR_GO_2ND_LINE = 0xc0,
        LCD_I2C_INSTRUCTION_ON_OFF_CTRL = 0x08,
        LCD_I2C_INSTRUCTION_CURSOR_OR_CHAR_SHIFT= 0x10,
    };


    lcd(unsigned int address, my_i2c the_i2c, lcd_fnc the_function = lcd_fnc::LCD_FunctionSet_4B_2L_510_F);
    int write_chars(std::string data);
    int read_chars(std::string data);
    int write_command(lcd_commands the_command);
private:

    enum class error_state{
        no_error,
        writing_data_error,
        writing_cmd_error,
        read_error,
        opening_error
    };

    error_state lcd_error_state;
    my_i2c the_used_i2c;
    lcd_fnc my_current_functionality;
    unsigned int lcd_address;


    int _initialize_lcd(void);
    int _write_cmd(unsigned char cmd);
    int _write_data(std::string data);
    int _read_cmd(void);
    int _read_data(void);
};
}

#endif