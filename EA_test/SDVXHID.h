#include "HID.h"

/* PINOUT - modify for your hardware */
#define BUT_PIN_START 10
#define BUT_PIN_A 11
#define BUT_PIN_B 9
#define BUT_PIN_C 7
#define BUT_PIN_D 5
#define BUT_PIN_FXL 6
#define BUT_PIN_FXR 8

// encoder pins
#define ENC_L_A 0
#define ENC_L_B 1
#define ENC_R_A 2
#define ENC_R_B 3

/* USER OPTIONS */
#define REPORT_DELAY 1 // HID report delay (ms)
#define MILLIDEBOUNCE 5

#define KONAMI_SPOOF 1 // enable Konami VID/PID spoofing

/* DO NOT EDIT BELOW */
#define EPTYPE_DESCRIPTOR_SIZE uint8_t
#define STRING_ID_Base 4

// LED disabled (using custom PCB)
#define NUM_BUT_LEDS 0
#define SIDE_NUM_LEDS 0

class SDVXHID_ : public PluggableUSBModule {

public:
  SDVXHID_(void);

  /**
   * Sends the gamepad button states to the PC as an HID report
   * param[in] buttonsState bitfield with currently pressed buttons
   * param[in] enc1 encoder 1 position
   * param[in] enc2 encoder 2 position
   * return USB_Send() return value
   */
  int sendState(uint32_t buttonsState, int32_t enc1, int32_t enc2);

protected:
  /* Implementation of the PUSBListNode */
  EPTYPE_DESCRIPTOR_SIZE epType[1];
  uint8_t protocol;
  uint8_t idle;
  int getInterface(uint8_t *interfaceCount);
  int getDescriptor(USBSetup &setup);
  bool setup(USBSetup &setup);
};

extern SDVXHID_ SDVXHID;
