  #include <libpynq.h>
  #include <stdio.h>
  #include <stdbool.h>
  #include <stdlib.h>

void destroyer (display_t *display);
void dis_bpm(display_t *display, FontxFile *font, uint8_t *val);

int main(void) {
  const uint8_t hbeat_pynq_addr = 0x10;
  uint32_t register_map[1] = {0};
  uint32_t heart_rate = 138;

  uint32_t previous_heart_rate = 0;  // != heart_rate so the LCD draws on startup

  bool slave_mode;

  display_t display;
  FontxFile font[2];
  const char *f0 = "/home/student/libpynq-5EWC0-2023-v0.2.6/fonts/ILGH24XB.FNT";

  char val_1[20] = "0";

  pynq_init();

  //interactive elements
  buttons_init();

  bool button_0_pushed = 0;
  bool button_1_pushed = 0;
  bool button_2_pushed = 0;
  bool button_3_pushed = 0;

  display_init(&display);

  InitFontx(font, f0, "");
  displaySetFontDirection(&display, TEXT_DIRECTION0);

  switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);
  switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
    
  iic_init(IIC0);

  register_map[0] = heart_rate;

  slave_mode = iic_set_slave_mode(IIC0, hbeat_pynq_addr, register_map, 1);

  if (!slave_mode)
  {
    printf("Heartbeat PYNQ wasn't succesfully set as a iic slave!\n");
    destroyer(&display);
    return EXIT_FAILURE;
  }

  else
  {
    printf("Heartbeat pynq is succesfully set as an iic slave\n");
  }

  while (1)
  {
    button_0_pushed = get_button_state(BUTTON0);
    button_1_pushed = get_button_state(BUTTON1);
    button_2_pushed = get_button_state(BUTTON2);
    button_3_pushed = get_button_state(BUTTON3);

    if (button_0_pushed)
    {
      heart_rate = 60;
    }
    
    else if (button_1_pushed)
    {
      heart_rate = 120;
    }

    else if (button_2_pushed)
    {
      heart_rate = 180;
    }

    else if (button_3_pushed)
    {
      heart_rate = 240;
    }

    register_map[0] = heart_rate;

    snprintf(val_1, sizeof(val_1), "%u", heart_rate);

    iic_slave_mode_handler(IIC0);

    if (heart_rate != previous_heart_rate)
    {
      printf("Heart Rate: %u\n", heart_rate);

      dis_bpm(&display, font, (uint8_t *)val_1);

      previous_heart_rate = heart_rate;
    }
  }


  destroyer(&display);
  return EXIT_SUCCESS;
}

void destroyer(display_t *display)
  {
    iic_destroy(IIC0);
    display_destroy(display);
    buttons_destroy();
    pynq_destroy();
  }

void dis_bpm(display_t *display, FontxFile *font, uint8_t *val)
{
    uint8_t parameter[] = "Heart Rate:";
    uint8_t unit[] = "BPM";

    displayDrawString(display, font, 10, 26, parameter, RGB_WHITE);
    displayDrawRect(display, 10, 28, 228, 54, RGB_WHITE);
    displayDrawFillRect(display, 40, 29, 99, 52, RGB_BLACK);
    displayDrawString(display, font, 40, 51, val, RGB_WHITE);
    displayDrawString(display, font, 110, 51, unit, RGB_WHITE);
}