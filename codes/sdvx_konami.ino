/*
 * SDVX Controller - Dual Mode (Konami HID + Keyboard)
 * Arduino Leonardo
 *
 * Mode 1 (default): Konami HID - Native SDVX EACloud support
 * Mode 2: Full Keyboard - For other rhythm games
 *
 * Mode Switch: Hold START + BT-B + BT-C for 3 seconds
 * LED Feedback: START button LED blinks 3 times on mode change
 */

#include "SDVXHID.h"
#include "config.h" // Pin configuration - EDIT THIS FILE FOR YOUR SETUP
#include <Bounce2.h>
#include <Encoder.h>
#include <Keyboard.h>

// Button pins array
const uint8_t buttonPins[] = {BUT_PIN_START, BUT_PIN_A,   BUT_PIN_B,  BUT_PIN_C,
                              BUT_PIN_D,     BUT_PIN_FXL, BUT_PIN_FXR};
#define NUM_BUTTONS 7

// Debounced button objects
Bounce buttons[NUM_BUTTONS];

// Rotary encoders
Encoder encL(ENC_L_A, ENC_L_B);
Encoder encR(ENC_R_A, ENC_R_B);

// Mode switching
bool keyboardMode = false; // false = HID mode, true = Keyboard mode
unsigned long modeButtonPressTime = 0;
bool modeButtonPressed = false;

// Encoder state for keyboard mode
long lastEncL = 0;
long lastEncR = 0;

void setup() {
  // Initialize buttons with debouncing
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].attach(buttonPins[i], INPUT_PULLUP);
    buttons[i].interval(MILLIDEBOUNCE);
  }

  // Initialize encoder pins
  pinMode(ENC_L_A, INPUT_PULLUP);
  pinMode(ENC_L_B, INPUT_PULLUP);
  pinMode(ENC_R_A, INPUT_PULLUP);
  pinMode(ENC_R_B, INPUT_PULLUP);

  encL.write(0);
  encR.write(0);

  Keyboard.begin();

  // Built-in LED shows current mode
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
      // 3 seconds pressed -> change mode
      keyboardMode = !keyboardMode;

      // Wait for button release
      while (digitalRead(BUT_PIN_START) == LOW ||
             digitalRead(BUT_PIN_B) == LOW || digitalRead(BUT_PIN_C) == LOW) {
        delay(10);
      }
      delay(100);

      // Release all keyboard keys to prevent stuck keys
      Keyboard.releaseAll();

      // START LED feedback (3 blinks)
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
  uint16_t buttonState = 0;

  // Read physical buttons
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].update();
  }

  // Map physical buttons to correct HID button numbers
  // Physical button order: START, BT-A, BT-B, BT-C, BT-D, FX-L, FX-R
  // HID mapping: 8, 0, 1, 2, 3, 4, 5
  if (buttons[0].read() == LOW)
    buttonState |= (1 << 8); // START -> Button 8
  if (buttons[1].read() == LOW)
    buttonState |= (1 << 0); // BT-A -> Button 0
  if (buttons[2].read() == LOW)
    buttonState |= (1 << 1); // BT-B -> Button 1
  if (buttons[3].read() == LOW)
    buttonState |= (1 << 2); // BT-C -> Button 2
  if (buttons[4].read() == LOW)
    buttonState |= (1 << 3); // BT-D -> Button 3
  if (buttons[5].read() == LOW)
    buttonState |= (1 << 4); // FX-L -> Button 4
  if (buttons[6].read() == LOW)
    buttonState |= (1 << 5); // FX-R -> Button 5

  checkModeSwitch();

  if (keyboardMode) {
    // mode 2 - Full keyboard mode (no HID)

    // Buttons as keyboard
    if (buttons[0].read() == LOW)
      Keyboard.press(KEY_RETURN);
    else
      Keyboard.release(KEY_RETURN); // START
    if (buttons[1].read() == LOW)
      Keyboard.press('d');
    else
      Keyboard.release('d'); // BT-A
    if (buttons[2].read() == LOW)
      Keyboard.press('f');
    else
      Keyboard.release('f'); // BT-B
    if (buttons[3].read() == LOW)
      Keyboard.press('j');
    else
      Keyboard.release('j'); // BT-C
    if (buttons[4].read() == LOW)
      Keyboard.press('k');
    else
      Keyboard.release('k'); // BT-D
    if (buttons[5].read() == LOW)
      Keyboard.press('c');
    else
      Keyboard.release('c'); // FX-L
    if (buttons[6].read() == LOW)
      Keyboard.press('m');
    else
      Keyboard.release('m'); // FX-R

    // Knobs as keyboard
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
    // mode 1 - HID mode for EACloud

    int32_t enc1 = encL.read();
    int32_t enc2 = encR.read();

    // Direct mapping for maximum speed (no bit shift)
    uint8_t axis_x = (uint8_t)(enc1 & 0xFF);
    uint8_t axis_y = (uint8_t)(enc2 & 0xFF);

    SDVXHID.sendState(buttonState, axis_x, axis_y);

    delay(REPORT_DELAY);
  }
}
