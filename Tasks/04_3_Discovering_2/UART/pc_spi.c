#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEVICE "/dev/spidev0.0"

void pabort(const char *s)
{
	perror(s);
	abort();
}

int main(void)
{
	int fd;
	uint8_t mode = SPI_MODE_0;
	uint8_t bits = 0;
	uint32_t speed = 50000;

	fd = open(SPI_DEVICE, O_RDWR);
	if (fd < 0)
		pabort("Can't open SPI device");
	
	if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1)
		pabort("Can't set SPI mode");

	if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1)
		pabort("Can't set pits per word");

	if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1)
		pabort("Can't set max speed hz");

	char data_to_send[] = "Hello";
	int length = strlen(data_to_send);

	struct spi_ioc_transfer transfer = {
		.tx_buf = (unsigned long)data_to_send,
		.len = length,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	if (ioctl(fd, SPI_IOC_MESSAGE(1), &transfer) < 0)
		pabort("Can't send SPI message");
	
	close(fd);

	return 0;


}
