// BETA (i can't test it konami maintanance shit.)


#include "SDVXHID.h"
#include <Bounce2.h>
#include <Encoder.h>
#include <Keyboard.h>

// Button Pin
const uint8_t buttonPins[] = {BUT_PIN_START, BUT_PIN_A,   BUT_PIN_B,  BUT_PIN_C,
                              BUT_PIN_D,     BUT_PIN_FXL, BUT_PIN_FXR};
#define NUM_BUTTONS 7

// keyboard keys
const char keyboardKeys[] = {KEY_RETURN, 'd', 'f', 'j', 'k', 'c', 'm'};

// Bounce
Bounce buttons[NUM_BUTTONS];

// encoder
Encoder encL(ENC_L_A, ENC_L_B);
Encoder encR(ENC_R_A, ENC_R_B);

// change mode
bool keyboardMode = false; // false = HID, true = Keyboard
unsigned long modeButtonPressTime = 0;
bool modeButtonPressed = false;

// Mode 2 - knob
long lastEncL = 0;
long lastEncR = 0;
#define KNOB_DEADZONE 2

void setup() {
  // reset button
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].attach(buttonPins[i], INPUT_PULLUP);
    buttons[i].interval(MILLIDEBOUNCE);
  }

  // Encoder Pullup
  pinMode(ENC_L_A, INPUT_PULLUP);
  pinMode(ENC_L_B, INPUT_PULLUP);
  pinMode(ENC_R_A, INPUT_PULLUP);
  pinMode(ENC_R_B, INPUT_PULLUP);

  encL.write(0);
  encR.write(0);

  Keyboard.begin();

  // internal led mode
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, keyboardMode ? HIGH : LOW);
}

void checkModeSwitch() {
  // START + B + C Mode change detect
  bool start = (buttons[0].read() == LOW);
  bool b = (buttons[2].read() == LOW);
  bool c = (buttons[3].read() == LOW);

  if (start && b && c) {
    if (!modeButtonPressed) {
      modeButtonPressTime = millis();
      modeButtonPressed = true;
    } else if (millis() - modeButtonPressTime > 3000) {
      // press 3s -> change mode
      keyboardMode = !keyboardMode;

      // waiting release 2 button
      while (digitalRead(BUT_PIN_START) == LOW ||
             digitalRead(BUT_PIN_B) == LOW || digitalRead(BUT_PIN_C) == LOW) {
        delay(10);
      }
      delay(100);

      // START Led feedback (3time)
      pinMode(BUT_PIN_START, OUTPUT);
      for (int i = 0; i < 3; i++) {
        digitalWrite(BUT_PIN_START, LOW);
        delay(150);
        digitalWrite(BUT_PIN_START, HIGH);
        delay(150);
      }
      pinMode(BUT_PIN_START, INPUT_PULLUP);

      modeButtonPressed = false;
    }
  } else {
    modeButtonPressed = false;
  }
}

void loop() {
  uint32_t buttonState = 0;
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].update();
    if (buttons[i].read() == LOW) {
      buttonState |= (1 << i);
    }
  }

  checkModeSwitch();

  if (keyboardMode) {
    // mode 2

    SDVXHID.sendState(buttonState, 0, 0);

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
    // mode 1

    int32_t enc1 = encL.read();
    int32_t enc2 = encR.read();

    SDVXHID.sendState(buttonState, enc1, enc2);

    delay(REPORT_DELAY);
  }
}
