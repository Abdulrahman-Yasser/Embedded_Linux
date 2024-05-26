#include <gpiod.h>        // All of the GPIO stuff...
#include <unistd.h>       // usleep()
int main(int argc, char **argv)
{
  const char *chipname = "gpiochip2";
  struct gpiod_chip *chip;
  struct gpiod_line *led;
  int state, counter;
  // Setup the GPIO Chip struct...
  chip = gpiod_chip_open_by_name(chipname);
  // Setup the GPIO "line"  (pin 14)
  led = gpiod_chip_get_line(chip, 0);
  // Set the LED "line" for output
  gpiod_line_request_output(led, "example1", 0);
  state = 0;
  // Now we'll blink the LED for a bit...
  for (counter=0;counter<100;counter++)
  {
    gpiod_line_set_value(led, state);
    state = !(state);
    // With a sleep of 100000 microseconds === 0.1 seconds
    usleep(100000);
  }
  // Release lines and chip & exit
  gpiod_line_set_value(led, 0);
  gpiod_line_release(led);
  gpiod_chip_close(chip);
  return 0;
}
