#include "TermioWrapper.hpp"
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

namespace my_wrapper
{

	TermioWrapper::TermioWrapper(std::string the_tty_path):
	my_tty_path(the_tty_path)
	{
		terminal_descriptor = open(my_tty_path.c_str(), O_RDWR | O_NOCTTY);
		tcgetattr(terminal_descriptor, &the_terminals_settings);
	}

	TermioWrapper::TermioWrapper(std::string the_tty_path, struct termios the_term_arg):
	my_tty_path(the_tty_path)
	{
		terminal_descriptor = open(my_tty_path.c_str(), O_RDWR | O_NOCTTY);

		/* Getting the termios options */
		memcpy((void*)&the_terminals_settings, (void*)&the_term_arg, sizeof(struct termios));
	}

	template <typename T>
	int TermioWrapper::change_configurations(T the_flag, char Enable_or_value)
	{
		/* Setting the Baud rate */

		if (std::is_same<T, TermioWrapper::C_flags>::value)
		{
			printf("\nYou Entered C_flag");
			the_terminals_settings.c_cflag &= ~(the_flag & 0xffff);
			the_terminals_settings.c_cflag |= (the_flag & Enable_or_value);
		}
		else if (std::is_same<T, TermioWrapper::L_flags>::value)
		{
			printf("\nYou Entered L_flag");
			the_terminals_settings.c_lflag &= ~(the_flag & 0xffff);
			the_terminals_settings.c_lflag |= (the_flag & Enable_or_value);
		}
		else if (std::is_same<T, TermioWrapper::C_cc_flags>::value)
		{
			printf("\nYou Entered C_cc_flag");
			the_terminals_settings.c_cc[the_flag] = Enable_or_value;
		}
		else if (std::is_same<T, TermioWrapper::I_flags>::value)
		{
			printf("\nYou Entered I_flag");
			the_terminals_settings.c_iflag &= ~(the_flag & 0xffff);
			the_terminals_settings.c_iflag |= (the_flag & Enable_or_value);
		}
		else if (std::is_same<T, TermioWrapper::O_flag>::value)
		{
			printf("\nYou Entered O_flag");
			the_terminals_settings.c_oflag &= ~(the_flag & 0xffff);
			the_terminals_settings.c_oflag |= (the_flag & Enable_or_value);
		}
		else
		{
			printf("\nYou can't enter that type !");
			return 0;
		}
		return 1;
	}

	int TermioWrapper::write_configuration_buffer(void)
	{
		int ret = (tcsetattr(terminal_descriptor, TCSANOW, &the_terminals_settings));
		if (ret != 0)
			printf("\n  ERROR ! in Setting attributes");
		else
			printf("\n  BaudRate = 9600 \n  StopBits = 1 \n  Parity   = none");
		return ret;
	}

	int TermioWrapper::sent_data(int date)
	{
		return 0;
	}

	int TermioWrapper::read_date(int &date)
	{
		return 0;
	}


}
