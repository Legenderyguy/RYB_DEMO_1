  #include <libpynq.h>
  #include <stdio.h>
  #include <stdbool.h>
  #include <stdlib.h>
  #include <string.h>

  void destroyer(display_t *display);
  void dis_bpm(display_t *display, FontxFile *font, uint8_t *val);
  void dis_amplitude(display_t *display, FontxFile *font, uint8_t *val);
  void dis_frequency(display_t *display, FontxFile *font, uint8_t *val);

  int main(void) {
    
    //heartbeat pynq variables
    uint32_t heart_rate = 0;
    const uint8_t heartbeat_pynq = 0x10;
    const uint8_t heart_rate_register = 0x00;
    uint16_t heart_rate_size = sizeof(heart_rate);
    bool error_heartbeat;

    //Actuator pynq variables
    uint32_t rocking_amplitude = 70;
    uint32_t rocking_frequency = 30;
    const uint8_t actuator_pynq = 0x30;
    const uint8_t rocking_amplitude_register = 0x00;
    const uint8_t rocking_frequency_register = 0x01;
    uint16_t rocking_amplitude_size = sizeof(rocking_amplitude);
    uint16_t rocking_frequency_size = sizeof(rocking_frequency);
    bool error_actuator_amplitude;
    bool error_actuator_frequency;

    display_t display;
    FontxFile font[2];

    const char *f0 = "/home/student/libpynq-5EWC0-2023-v0.2.6/fonts/ILGH24XB.FNT";

    char val_1[20] = "0";
    char val_3[20] = "31";
    char val_4[20] = "0.69";
    
    pynq_init();

    switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA); //TODO check if you can use IO_AR_SCL
    switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL); //TODO check if you can use IO_AR_SCL

    //interactive elements
    buttons_init();

    bool button_0_pushed = 0;
    bool button_1_pushed = 0;
    bool button_2_pushed = 0;
    bool button_3_pushed = 0;

    iic_init(IIC0);
    iic_reset(IIC0);

    display_init(&display);
    
    InitFontx(font, f0, "");
    displaySetFontDirection(&display, TEXT_DIRECTION0);

    while (1) {
      button_0_pushed = get_button_state(BUTTON0);
      button_1_pushed = get_button_state(BUTTON1);
      button_2_pushed = get_button_state(BUTTON2);
      button_3_pushed = get_button_state(BUTTON3);

      // Read
      error_heartbeat = iic_read_register(IIC0, heartbeat_pynq, heart_rate_register, (uint8_t *)&heart_rate, heart_rate_size);

      if (error_heartbeat) //the iic_read_register function returns 0 if successful and 1 on error. So it says whether there was error (1), or not(0);
      {
        printf("Master couldn't read the value from the heartbeat pynq\n");
        destroyer(&display);
        return EXIT_FAILURE;
      }


      snprintf(val_1, sizeof(val_1), "%u", heart_rate);

      if (button_0_pushed)
      {
        rocking_amplitude = 20;
        rocking_frequency = 15;
      }
      
      else if (button_1_pushed)
      {
        rocking_amplitude = 40;
        rocking_frequency = 35;
      }

      else if (button_2_pushed)
      {
        rocking_amplitude = 60;
        rocking_frequency = 55;
      }

      else if (button_3_pushed)
      {
        rocking_amplitude = 80;
        rocking_frequency = 75;
      }

      //Write
      error_actuator_amplitude = iic_write_register(IIC0, actuator_pynq, rocking_amplitude_register, (uint8_t *)&rocking_amplitude, rocking_amplitude_size);
      error_actuator_frequency = iic_write_register(IIC0, actuator_pynq, rocking_frequency_register, (uint8_t *)&rocking_frequency, rocking_frequency_size);

      if (error_actuator_amplitude)
      {
        printf("Master couldn't write the amplitude value to the actuator detection pynq\n");
        destroyer(&display);
        return EXIT_FAILURE;
      }

      if (error_actuator_frequency)
      {
        printf("Master couldn't write the frequency value to the actuator detection pynq\n");
        destroyer(&display);
        return EXIT_FAILURE;
      }

      snprintf(val_3, sizeof(val_3), "%u", rocking_amplitude);
      snprintf(val_4, sizeof(val_4), "%.2f", rocking_frequency / 100.0);

      //printed values
      printf("Heart rate: %u\n", heart_rate);

      //display functions
      dis_bpm(&display, font, (uint8_t *)val_1);
      dis_amplitude(&display, font, (uint8_t *)val_3);
      dis_frequency(&display, font, (uint8_t *)val_4);

      sleep_msec(50);
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
