/* SDVX Controller - Dual Mode (HID + Hybrid)
 * Arduino Leonardo
 *
 * 모드 전환: START + B + C 동시 누름 (3초)
 * - 모드 1: HID 전용 (EACloud) - 버튼+노브 모두 HID
 * - 모드 2: 하이브리드 - 버튼 HID, 노브 키보드
 */

#include "SDVXHID.h"
#include <Bounce2.h>
#include <Encoder.h>
#include <Keyboard.h>

// 버튼 핀 배열
const uint8_t buttonPins[] = {BUT_PIN_START, BUT_PIN_A,   BUT_PIN_B,  BUT_PIN_C,
                              BUT_PIN_D,     BUT_PIN_FXL, BUT_PIN_FXR};
#define NUM_BUTTONS 7

// 키보드 모드 키 매핑
const char keyboardKeys[] = {KEY_RETURN, 'd', 'f', 'j', 'k', 'c', 'm'};

// Bounce 객체 배열
Bounce buttons[NUM_BUTTONS];

// 엔코더
Encoder encL(ENC_L_A, ENC_L_B);
Encoder encR(ENC_R_A, ENC_R_B);

// 모드 전환
bool keyboardMode = false; // false = HID, true = Keyboard
unsigned long modeButtonPressTime = 0;
bool modeButtonPressed = false;

// 키보드 모드 - 노브
long lastEncL = 0;
long lastEncR = 0;
#define KNOB_DEADZONE 2

void setup() {
  // 버튼 초기화
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].attach(buttonPins[i], INPUT_PULLUP);
    buttons[i].interval(MILLIDEBOUNCE);
  }

  // 엔코더 핀 풀업
  pinMode(ENC_L_A, INPUT_PULLUP);
  pinMode(ENC_L_B, INPUT_PULLUP);
  pinMode(ENC_R_A, INPUT_PULLUP);
  pinMode(ENC_R_B, INPUT_PULLUP);

  encL.write(0);
  encR.write(0);

  Keyboard.begin();

  // LED로 모드 표시 (내장 LED)
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, keyboardMode ? HIGH : LOW);
}

void checkModeSwitch() {
  // START + B + C 동시 누름 감지
  bool start = (buttons[0].read() == LOW);
  bool b = (buttons[2].read() == LOW);
  bool c = (buttons[3].read() == LOW);

  if (start && b && c) {
    if (!modeButtonPressed) {
      modeButtonPressTime = millis();
      modeButtonPressed = true;
    } else if (millis() - modeButtonPressTime > 3000) {
      // 3초 이상 누름 → 모드 전환
      keyboardMode = !keyboardMode;

      // 버튼에서 손 뗄 때까지 대기
      while (digitalRead(BUT_PIN_START) == LOW ||
             digitalRead(BUT_PIN_B) == LOW || digitalRead(BUT_PIN_C) == LOW) {
        delay(10);
      }
      delay(100);

      // START 버튼 LED로 피드백
      pinMode(BUT_PIN_START, OUTPUT);
      for (int i = 0; i < 3; i++) {
        digitalWrite(BUT_PIN_START, LOW); // LED 켜기
        delay(150);
        digitalWrite(BUT_PIN_START, HIGH); // LED 끄기
        delay(150);
      }
      pinMode(BUT_PIN_START, INPUT_PULLUP); // 다시 버튼 입력 모드로

      modeButtonPressed = false;
    }
  } else {
    modeButtonPressed = false;
  }
}

void loop() {
  // 버튼 상태 읽기
  uint32_t buttonState = 0;
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].update();
    if (buttons[i].read() == LOW) {
      buttonState |= (1 << i);
    }
  }

  // 모드 전환 체크
  checkModeSwitch();

  if (keyboardMode) {
    // === 모드 2: 하이브리드 (버튼 HID, 노브 키보드) ===

    // 버튼은 HID로 전송
    SDVXHID.sendState(buttonState, 0, 0);

    // 노브는 키보드로 처리
    long enc1Val = encL.read();
    long delta1 = enc1Val - lastEncL;

    if (abs(delta1) > KNOB_DEADZONE) {
      if (delta1 > 0) {
        Keyboard.press('w');
        delay(5);
        Keyboard.release('w');
      } else {
        Keyboard.press('q');
        delay(5);
        Keyboard.release('q');
      }
      lastEncL = enc1Val;
    }

    if (abs(enc1Val) > 500) {
      encL.write(0);
      lastEncL = 0;
    }

    long enc2Val = encR.read();
    long delta2 = enc2Val - lastEncR;

    if (abs(delta2) > KNOB_DEADZONE) {
      if (delta2 > 0) {
        Keyboard.press('p');
        delay(5);
        Keyboard.release('p');
      } else {
        Keyboard.press('o');
        delay(5);
        Keyboard.release('o');
      }
      lastEncR = enc2Val;
    }

    if (abs(enc2Val) > 500) {
      encR.write(0);
      lastEncR = 0;
    }

    delay(REPORT_DELAY);

  } else {
    // === 모드 1: HID 전용 (EACloud) ===

    int32_t enc1 = encL.read();
    int32_t enc2 = encR.read();

    SDVXHID.sendState(buttonState, enc1, enc2);

    delay(REPORT_DELAY);
  }
}
