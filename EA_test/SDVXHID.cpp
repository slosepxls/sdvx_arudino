#include "SDVXHID.h"

/* HID DESCRIPTOR - NEMSYS 호환 */
static const byte PROGMEM _hidReportSDVX[] = {
    0x05, 0x01, // Usage Page (Generic Desktop Ctrls)
    0x09, 0x04, // Usage (Joystick)
    0xA1, 0x01, // Collection (Application)
    0x85, 0x04, //   Report ID (4)

    /* 9 buttons */
    0x05, 0x09, //   Usage Page (Button)
    0x19, 0x01, //   Usage Minimum (0x01)
    0x29, 0x09, //   Usage Maximum (0x09)
    0x15, 0x00, //   Logical Minimum (0)
    0x25, 0x01, //   Logical Maximum (1)
    0x95, 0x09, //   Report Count (9)
    0x75, 0x01, //   Report Size (1)
    0x81, 0x02, //   Input (Data,Var,Abs)

    /* 7 bits padding */
    0x95, 0x01, //   Report Count (1)
    0x75, 0x07, //   Report Size (7)
    0x81, 0x03, //   Input (Const,Var,Abs)

    /* 2 knobs as analog axis (8-bit) */
    0x05, 0x01,       //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x01,       //   Usage (Pointer)
    0x15, 0x00,       //   Logical Minimum (0)
    0x26, 0xFF, 0x00, //   Logical Maximum (255)
    0x95, 0x02,       //   Report Count (2)
    0x75, 0x08,       //   Report Size (8)
    0xA1, 0x00,       //   Collection (Physical)
    0x09, 0x30,       //     Usage (X)
    0x09, 0x31,       //     Usage (Y)
    0x81, 0x02,       //     Input (Data,Var,Abs)
    0xC0,             //   End Collection

    0xC0 // End Collection (Joystick)
};

#if KONAMI_SPOOF == 1
const DeviceDescriptor PROGMEM USB_DeviceDescriptorIAD =
    D_DEVICE(0xEF, 0x02, 0x01, 64, 0x1ccf, 0x101c, 0x100, IMANUFACTURER,
             IPRODUCT, ISERIAL, 1);
const char *const PROGMEM String_Manufacturer = "Konami Amusement";
const char *const PROGMEM String_Product = "SOUND VOLTEX controller";
#else
const char *const PROGMEM String_Manufacturer = "CrazyRedMachine";
const char *const PROGMEM String_Product = "SDVX Controller";
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

int SDVXHID_::sendState(uint32_t buttonsState, int32_t enc1, int32_t enc2) {
  /* 엔코더 값 필터링 (작은 변화 무시) */
  static int32_t prev_enc1 = 0;
  static int32_t prev_enc2 = 0;
  int32_t delta1 = enc1 - prev_enc1;
  int32_t delta2 = enc2 - prev_enc2;

  if (delta1 >= -15 && delta1 <= 15)
    enc1 = prev_enc1;
  if (delta2 >= -15 && delta2 <= 15)
    enc2 = prev_enc2;

  prev_enc1 = enc1;
  prev_enc2 = enc2;

  /* HID 리포트 전송 */
  uint8_t data[5];
  data[0] = (uint8_t)4; // report id
  data[1] = (uint8_t)(buttonsState & 0xFF);
  data[2] = (uint8_t)(buttonsState >> 8) & 0xFF;
  data[3] = (uint8_t)(enc1 >> 2 & 0xFF); // 10비트 → 8비트
  data[4] = (uint8_t)(enc2 >> 2 & 0xFF);

  return USB_Send(pluggedEndpoint | TRANSFER_RELEASE, data, 5);
}

SDVXHID_ SDVXHID;
