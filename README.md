# Arduino SDVX EAcloud

[English](README.en.md) | **한국어**

Arduino Leonardo 기반 SDVX 코드입니다. USB VID/PID 스푸핑으로 **코나스테 EAcloud 네이티브 지원**합니다.
IST MALL 아두이노 레오나르도 기반 SDVX 컨트롤러 사용했습니다.

Arduino Leonardo + IST SDVX PCB
로터리 엔코더

## 주요 기능

- **Konami USB VID/PID 스푸핑** (`0x1ccf:0x101c`) - 코나스테 네이티브 인식
- **모드 변경**
  - **모드 1 (HID)**: EAC 모드
  - **모드 2 (하이브리드)**: HID 모드 (버튼 - 조이스틱, 노브 - 키보드)
- **1000Hz 폴링**


## 핀 구성 (본인 환경에 맞게 수정)

### 버튼
| 버튼   | 핀  |
|--------|-----|
| START  | 10  |
| BT-A   | 11  |
| BT-B   | 9   |
| BT-C   | 7   |
| BT-D   | 5   |
| FX-L   | 6   |
| FX-R   | 8   |

### 엔코더
| 엔코더 | A 핀 | B 핀 |
|--------|------|------|
| VOL-L  | 0    | 1    |
| VOL-R  | 2    | 3    |

## 필수 라이브러리

1. **Encoder** by Paul Stoffregen
2. **Bounce2** by Thomas O Fredericks

### 모드 전환

**START + BT-B + BT-C 3초간 누르면** 모드 전환됩니다.
전환시 START 버튼 LED 3번 깜빡임

- **모드 1 (기본)**: HID 조이스틱 - 모든 입력 HID (코나스테 EAC용)
- **모드 2**: 하이브리드 - 버튼 HID, 노브 키보드 (q/w, o/p)

### 버튼 매핑


### License

MIT License

### Credits

KONAMI IO [CrazyRedMachine/SoundVoltexIO](https://github.com/CrazyRedMachine/SoundVoltexIO)
