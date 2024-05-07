#!/usr/bin/g++

#ifndef _TERMIOS_WRAPPER__H
#define _TERMIOS_WRAPPER__H



#include <string>
#include <termios.h>

namespace my_wrapper
{

	class TermioWrapper
	{
	public:
		enum Speed_Options{
			/* Baud Rate */
			B_0 = B0, B_50 = B50, B_75 = B75, B_110 = B110, B_134 = B134, B_150 = B150, B_200 = B200, B_300 = B300, B_600 = B600, B_1200 = B1200, B_1800 = B1800, B_2400 = B2400, B_4800 = B4800, B_9600 = B9600, B_19200 = B19200, B_38400 = B38400, 
			B_57600 = B57600, B_115200 = B115200, B_230400 = B230400, B_460800 = B460800, B_500000 = B500000, B_576000 = B576000, B_921600 = B921600, 
			B_1000000 = B1000000, B_1152000 = B1152000, B_1500000 = B1500000, B_2000000 = B2000000, B_2500000 = B2500000, B_3000000 = B3000000, B_3500000 = B3500000, B_4000000 = B4000000
		};

		enum C_flags
		{ 	
			C_SIZE = CSIZE, C_S5 = CS5, C_S6 = CS6, C_S7 = CS7, C_S8 = CS8, C_STOPB = CSTOPB, 
			C_READ = CREAD, C_PARENB = PARENB, C_PARODD = PARODD, H_UPCL = HUPCL, C_LOCAL = CLOCAL,
			C_IBAUD = CIBAUD, C_MSPAR = CMSPAR//, C_RTSCTS = CRTSCTS
		};

		enum L_flags
		{
			L_ISIG = ISIG,
			L_ICANON = ICANON,
#if defined __USE_MISC || (defined __USE_XOPEN && !defined __USE_XOPEN2K)
			L_XCASE = XCASE,
#endif
			L_ECHO = ECHO,
			L_ECHOE = ECHOE,
			L_ECHOK = ECHOK,
			L_ECHONL = ECHONL,
			L_NOFLSH = NOFLSH,
			L_TOSTOP = TOSTOP,
#ifdef __USE_MISC
			L_ECHOCTL = ECHOCTL,
			L_ECHOPRT = ECHOPRT,
			L_ECHOKE = ECHOKE,
			L_FLUSHO = FLUSHO,
			L_PENDIN = PENDIN,
#endif
			L_IEXTEN = IEXTEN,
#ifdef __USE_MISC
			L_EXTPROC = EXTPROC
#endif
		};

		enum C_cc_flags
		{
			CC_VINTR = VINTR, CC_VQUIT = VQUIT, CC_VERASE = VERASE, CC_VKILL = VKILL, CC_VEOF = VEOF, CC_VTIME = VTIME, CC_VMIN = VMIN, CC_VSWTC = VSWTC, CC_VSTART = VSTART, CC_VSTOP = VSTOP, 
			CC_VSUSP = VSUSP, CC_VEOL = VEOL, CC_VREPRINT = VREPRINT, CC_VDISCARD = VDISCARD, CC_VWERASE = VWERASE, CC_VLNEXT = VLNEXT, CC_VEOL2 = VEOL2 		
		};

		enum I_flags
		{
			I_IGNBRK = IGNBRK, I_BRKINT = BRKINT, I_IGNPAR = IGNPAR, I_PARMRK = PARMRK, I_INPCK = INPCK,
			I_ISTRIP = ISTRIP, I_INLCR = INLCR, I_IGNCR = IGNCR, I_ICRNL = ICRNL, I_IUCLC = IUCLC,
			I_IXON = IXON, I_IXANY = IXANY, I_IXOFF = IXOFF, I_IMAXBEL = IMAXBEL, I_IUTF8 = IUTF8
		};

		enum O_flag
		{
			O_OPOST = OPOST, O_OLCUC = OLCUC, O_ONLCR = ONLCR, O_OCRNL = OCRNL, O_ONOCR = ONOCR, O_ONLRET = ONLRET, 
			O_OFILL = OFILL, O_OFDEL = OFDEL, O_NLDLY = NLDLY, O_NL0 = NL0, O_NL1 = NL1, O_CRDLY = CRDLY, O_CR0 = CR0, 
			O_CR1 = CR1, O_CR2 = CR2, O_CR3 = CR3, O_TABDLY = TABDLY, O_TAB0 = TAB0, O_TAB1 = TAB1, O_TAB2 = TAB2, 
			O_TAB3 = TAB3, O_XTABS = XTABS, O_BSDLY = BSDLY, O_BS0 = BS0, O_BS1 = BS1, O_VTDLY = VTDLY, O_VT0 = VT0,
			O_VT1 = VT1, O_FFDLY = FFDLY, O_FF0 = FF0, O_FF1 = FF1
		};

		TermioWrapper(std::string);
		TermioWrapper(std::string, struct termios);
		~TermioWrapper();
		int sent_data(const char* data);
		int read_data(char* data);
		
		template <typename T>

		int change_configurations(T the_flag, unsigned int Enable_or_value)
		{
			/* Setting the Baud rate */
			if(Enable_or_value != 0){
				Enable_or_value=0xffffffff;
			}
			if (std::is_same<T, TermioWrapper::C_flags>::value)
			{
				printf("\nYou Entered C_flag");
				the_terminals_settings.c_cflag &= ~(the_flag);
				the_terminals_settings.c_cflag |= (the_flag & Enable_or_value);
			}
			else if (std::is_same<T, TermioWrapper::L_flags>::value)
			{
				printf("\nYou Entered L_flag");
				the_terminals_settings.c_lflag &= ~(the_flag);
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
				the_terminals_settings.c_iflag &= ~(the_flag);
				the_terminals_settings.c_iflag |= (the_flag & Enable_or_value);
			}
			else if (std::is_same<T, TermioWrapper::O_flag>::value)
			{
				printf("\nYou Entered O_flag");
				the_terminals_settings.c_oflag &= ~(the_flag);
				the_terminals_settings.c_oflag |= (the_flag & Enable_or_value);
			}
			else if (std::is_same<T, TermioWrapper::Speed_Options>::value)
			{
				printf("\nYou Entered Speed choice");
				cfsetispeed(&the_terminals_settings, the_flag);
				cfsetospeed(&the_terminals_settings, the_flag);
			}
			else
			{
				printf("\nYou can't enter that type !");
				return 0;
			}
			return 1;
		}

		void raw_setting(void);

		int write_configuration_buffer(void);
	private:
		std::string my_tty_path;
		int terminal_descriptor;
		struct termios the_terminals_settings;
	};

}


#endif
