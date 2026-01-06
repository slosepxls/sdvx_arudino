#include "SDVXHID.h"

/* HID DESCRIPTOR - Real Konami NEMSYS (16 buttons, 2 axes) */
static const byte PROGMEM _hidReportSDVX[] = {
    0x05, 0x01, // Usage Page (Generic Desktop)
    0x09, 0x04, // Usage (Joystick)
    0xA1, 0x01, // Collection (Application)

    0x05, 0x09, //   Usage Page (Button)
    0x19, 0x01, //   Usage Minimum (Button 1)
    0x29, 0x10, //   Usage Maximum (Button 16)
    0x15, 0x00, //   Logical Minimum (0)
    0x25, 0x01, //   Logical Maximum (1)
    0x95, 0x10, //   Report Count (16)
    0x75, 0x01, //   Report Size (1)
    0x81, 0x02, //   Input (Data,Var,Abs)

    0x05, 0x01,       //   Usage Page (Generic Desktop)
    0x09, 0x30,       //   Usage (X)
    0x09, 0x31,       //   Usage (Y)
    0x15, 0x00,       //   Logical Minimum (0)
    0x26, 0xFF, 0x00, //   Logical Maximum (255)
    0x75, 0x08,       //   Report Size (8)
    0x95, 0x02,       //   Report Count (2)
    0x81, 0x02,       //   Input (Data,Var,Abs)

    0xC0 // End Collection
};

#if KONAMI_SPOOF == 1
const DeviceDescriptor PROGMEM USB_DeviceDescriptorIAD =
    D_DEVICE(0xEF, 0x02, 0x01, 64, 0x1ccf, 0x101c, 0x100, IMANUFACTURER,
             IPRODUCT, ISERIAL, 1);
const char *const PROGMEM String_Manufacturer = "Konami Amusement";
const char *const PROGMEM String_Product = "SOUND VOLTEX controller";
#else
const char *const PROGMEM String_Manufacturer = "Arduino";
const char *const PROGMEM String_Product = "SDVX Test";
#endif
const char *const PROGMEM String_Serial = "SDVX";

static bool SendControl(uint8_t d) { return USB_SendControl(0, &d, 1) == 1; }

static bool USB_SendStringDescriptor(const char *string_P, uint8_t string_len,
                                     uint8_t flags) {
  SendControl(2 + string_len * 2);
  SendControl(3);
  bool pgm = flags & TRANSFER_PGM;
  for (uint8_t i = 0; i < string_len; i++) {
    bool r = SendControl(pgm ? pgm_read_byte(&string_P[i]) : string_P[i]);
    r &= SendControl(0);
    if (!r) {
      return false;
    }
  }
  return true;
}

SDVXHID_::SDVXHID_(void) : PluggableUSBModule(1, 1, epType) {
  epType[0] = EP_TYPE_INTERRUPT_IN;
  PluggableUSB().plug(this);
}

int SDVXHID_::getInterface(byte *interfaceCount) {
  *interfaceCount += 1;
  HIDDescriptor hidInterface = {
      D_INTERFACE(pluggedInterface, 1, USB_DEVICE_CLASS_HUMAN_INTERFACE,
                  HID_SUBCLASS_NONE, HID_PROTOCOL_NONE),
      D_HIDREPORT(sizeof(_hidReportSDVX)),
      D_ENDPOINT(USB_ENDPOINT_IN(pluggedEndpoint), USB_ENDPOINT_TYPE_INTERRUPT,
                 USB_EP_SIZE, 0x01)};
  return USB_SendControl(0, &hidInterface, sizeof(hidInterface));
}

int SDVXHID_::getDescriptor(USBSetup &setup) {
#if KONAMI_SPOOF == 1
  if (setup.wValueH == USB_DEVICE_DESCRIPTOR_TYPE) {
    return USB_SendControl(TRANSFER_PGM,
                           (const uint8_t *)&USB_DeviceDescriptorIAD,
                           sizeof(USB_DeviceDescriptorIAD));
  }
#endif
  if (setup.wValueH == USB_STRING_DESCRIPTOR_TYPE) {
    if (setup.wValueL == IPRODUCT) {
      return USB_SendStringDescriptor(String_Product, strlen(String_Product),
                                      0);
    } else if (setup.wValueL == IMANUFACTURER) {
      return USB_SendStringDescriptor(String_Manufacturer,
                                      strlen(String_Manufacturer), 0);
    } else if (setup.wValueL == ISERIAL) {
      return USB_SendStringDescriptor(String_Serial, strlen(String_Serial), 0);
    }
  }

  if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE) {
    return 0;
  }
  if (setup.wValueH != HID_REPORT_DESCRIPTOR_TYPE) {
    return 0;
  }
  if (setup.wIndex != pluggedInterface) {
    return 0;
  }

  return USB_SendControl(TRANSFER_PGM, _hidReportSDVX, sizeof(_hidReportSDVX));
}

bool SDVXHID_::setup(USBSetup &setup) {
  if (pluggedInterface != setup.wIndex) {
    return false;
  }
  return false;
}

int SDVXHID_::sendState(uint16_t buttonsState, uint8_t axis_x, uint8_t axis_y) {
  uint8_t data[4];
  data[0] = (uint8_t)(buttonsState & 0xFF);      // buttons 1-8
  data[1] = (uint8_t)(buttonsState >> 8) & 0xFF; // buttons 9-16
  data[2] = axis_x;                              // X axis (knob L)
  data[3] = axis_y;                              // Y axis (knob R)

  return USB_Send(pluggedEndpoint | TRANSFER_RELEASE, data, 4);
}

SDVXHID_ SDVXHID;
