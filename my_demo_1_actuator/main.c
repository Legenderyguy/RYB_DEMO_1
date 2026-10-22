#include <libpynq.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

void destroyer (display_t *display);
void dis_amplitude(display_t *display, FontxFile *font, uint8_t *val);
void dis_frequency(display_t *display, FontxFile *font, uint8_t *val);


int main(void) {
  const uint8_t actuator_pynq_addr = 0x30;
  uint32_t register_map[2] = {0, 0};
  uint32_t rocking_amplitude = 0;
  uint32_t rocking_frequency = 0;

  uint32_t previous_amplitude = 0;
  uint32_t previous_frequency = 0;

  bool slave_mode;

  display_t display;
  FontxFile font[2];
  const char *f0 = "/home/student/libpynq-5EWC0-2023-v0.2.6/fonts/ILGH24XB.FNT";

  char val_1[20] = "0";
  char val_2[20] = "0";

  pynq_init();

  display_init(&display);

  InitFontx(font, f0, "");
  displaySetFontDirection(&display, TEXT_DIRECTION0);

  switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA); //TODO check if you can use IO_AR_SCL
  switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL); //TODO check if you can use IO_AR_SCL

  iic_init(IIC0);

  slave_mode = iic_set_slave_mode(IIC0, actuator_pynq_addr, register_map, 2);

  if(!slave_mode)
  {
    printf("Actuator pynq couldn't be set as an iic slave!\n");
    destroyer(&display);
    return EXIT_FAILURE;
  }
  else
    printf("Actuator pynq is succesfully set as an iic slave\n");

  while(1)
  {
    iic_slave_mode_handler(IIC0);

    rocking_amplitude = register_map[0];
    rocking_frequency = register_map[1];

    snprintf(val_1, sizeof(val_1), "%u", rocking_amplitude);
    snprintf(val_2, sizeof(val_2), "%.2f", rocking_frequency / 100.0);

    if (rocking_amplitude != previous_amplitude ||
        rocking_frequency != previous_frequency)
    {
      printf("Amplitude: %u\n", rocking_amplitude);
      printf("Frequency: %u\n", rocking_frequency);

      dis_amplitude(&display, font, (uint8_t *)val_1);
      dis_frequency(&display, font, (uint8_t *)val_2);

      previous_amplitude = rocking_amplitude;
      previous_frequency = rocking_frequency;
    }
  }

  destroyer(&display);
  return EXIT_SUCCESS;
}

void destroyer(display_t *display)
  {
    iic_destroy(IIC0);
    display_destroy(display);
    pynq_destroy();
  }

  void dis_amplitude(display_t *display, FontxFile *font, uint8_t *val)
{
    uint8_t parameter[] = "Rocking Amplitude:";
    uint8_t unit[] = "%";

    displayDrawString(display, font, 10, 130, parameter, RGB_WHITE);
    displayDrawRect(display, 10, 132, 228, 158, RGB_WHITE);
    displayDrawFillRect(display, 40, 133, 99, 156, RGB_BLACK);
    displayDrawString(display, font, 40, 155, val, RGB_WHITE);
    displayDrawString(display, font, 110, 155, unit, RGB_WHITE);
}

void dis_frequency(display_t *display, FontxFile *font, uint8_t *val)
{
    uint8_t parameter[] = "Rocking Frequency:";
    uint8_t unit[] = "Hz";

    displayDrawString(display, font, 10, 182, parameter, RGB_WHITE);
    displayDrawRect(display, 10, 184, 228, 210, RGB_WHITE);
    displayDrawFillRect(display, 40, 185, 99, 208, RGB_BLACK);
    displayDrawString(display, font, 40, 207, val, RGB_WHITE);
    displayDrawString(display, font, 110, 207, unit, RGB_WHITE);
}