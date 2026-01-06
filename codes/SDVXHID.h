#include "HID.h"
#include "config.h" // User-configurable pin settings

/* DO NOT EDIT BELOW */
#define EPTYPE_DESCRIPTOR_SIZE uint8_t

class SDVXHID_ : public PluggableUSBModule {
public:
  SDVXHID_(void);
  int sendState(uint16_t buttonsState, uint8_t axis_x, uint8_t axis_y);

protected:
  EPTYPE_DESCRIPTOR_SIZE epType[1];
  uint8_t protocol;
  uint8_t idle;
  int getInterface(uint8_t *interfaceCount);
  int getDescriptor(USBSetup &setup);
  bool setup(USBSetup &setup);
};

extern SDVXHID_ SDVXHID;
