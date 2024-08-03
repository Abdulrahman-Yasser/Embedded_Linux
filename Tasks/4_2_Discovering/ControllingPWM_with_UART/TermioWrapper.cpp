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
		terminal_descriptor = open(my_tty_path.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
		tcgetattr(terminal_descriptor, &the_terminals_settings);
	}

	TermioWrapper::TermioWrapper(std::string the_tty_path, struct termios the_term_arg):
	my_tty_path(the_tty_path)
	{
		terminal_descriptor = open(my_tty_path.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

		/* Getting the termios options */
		the_terminals_settings.c_iflag = the_term_arg.c_iflag;		/* input mode flags */
		the_terminals_settings.c_oflag = the_term_arg.c_oflag;		/* output mode flags */
		the_terminals_settings.c_cflag = the_term_arg.c_cflag;		/* control mode flags */
		the_terminals_settings.c_lflag = the_term_arg.c_lflag;		/* local mode flags */
		the_terminals_settings.c_line = the_term_arg.c_line;			/* line discipline */
		the_terminals_settings.c_ispeed = the_term_arg.c_ispeed;		/* input speed */
		the_terminals_settings.c_ospeed = the_term_arg.c_ospeed;		/* output speed */

		memcpy((void*)&the_terminals_settings.c_cc, (void*)&the_term_arg.c_cc, sizeof(NCCS)); /* control characters */
	}

	TermioWrapper::~TermioWrapper()
	{
		close(terminal_descriptor);
	}


	int TermioWrapper::write_configuration_buffer(void)
	{
		tcflush(terminal_descriptor, TCIFLUSH);
		int ret = (tcsetattr(terminal_descriptor, TCSANOW, &the_terminals_settings));
		if (ret != 0)
			printf("\n  ERROR ! in Setting attributes");
		return ret;
	}

	int TermioWrapper::sent_data(const char* write_buffer)
	{
		if(terminal_descriptor != -1){
			int bytes_written = 0;
			printf("Data length : %d\n", (int)strlen(write_buffer));
			bytes_written = write(terminal_descriptor, write_buffer, strlen(write_buffer));
			printf("Bytes written to ttyUSB0 : %d\n", bytes_written);
			printf("%s written to ttyUSB0\n",write_buffer);
			return bytes_written;
		}
		return 0;
	}

	int TermioWrapper::read_data(char* read_buffer)
	{
		if(terminal_descriptor != -1){
			memset(read_buffer, 0, 255);
			int bytes_read = read(terminal_descriptor, &read_buffer, 255);

			printf("Bytes Rxed -%d\n", bytes_read); /* Print the number of bytes read */
			printf("Data : %s \n", read_buffer);
			return bytes_read;
		}
		return 0;
	}

	void TermioWrapper::raw_setting(void){
		cfmakeraw(&the_terminals_settings);
	}


}
