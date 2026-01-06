# Arduino SDVX EAcloud

**English** | [한국어](README.md)

Arduino Leonardo based SDVX code with **KONASTE EAcloud native support** via USB VID/PID spoofing.
Used IST MALL Arduino Leonardo based SDVX controller.

Arduino Leonardo + IST SDVX PCB
Rotary Encoders

## Features

- **Konami USB VID/PID Spoofing** (`0x1ccf:0x101c`) - Native KONASTE recognition
- **Dual Mode Operation**
  - **Mode 1 (HID)**: EAC Mode
  - **Mode 2 (Hybrid)**: HID Mode (Button - joystick, Knob - keyboard input)
- **1000Hz Polling Rate**


## Pin Configuration (Modify to match your setup config.h)

### Buttons
| Button | Pin |
|--------|-----|
| START  | 10  |
| BT-A   | 11  |
| BT-B   | 9   |
| BT-C   | 7   |
| BT-D   | 5   |
| FX-L   | 6   |
| FX-R   | 8   |

### Encoders
| Encoder | A Pin | B Pin |
|---------|-------|-------|
| VOL-L   | 0     | 1     |
| VOL-R   | 2     | 3     |

## Required Libraries

1. **Encoder** by Paul Stoffregen
2. **Bounce2** by Thomas O Fredericks

### Mode Switching

**Hold START + BT - B + BT - C for 3 seconds** to toggle between modes.
START button LED feedback on mode switch

- **Mode 1 (Default)**: HID Joystick - All inputs via HID (KONASTE EAC)
- **Mode 2**: Hybrid - Buttons via HID, Knobs via Keyboard (q/w, o/p)

**Visual Feedback**: START button LED blinks 3 times after mode switch.

### Button Mapping


### License

MIT License

### Credits

KONAMI IO [CrazyRedMachine/SoundVoltexIO](https://github.com/CrazyRedMachine/SoundVoltexIO)
