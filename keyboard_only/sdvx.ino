// keyboard only mode

#define BT_A 11
#define BT_B 9
#define BT_C 7
#define BT_D 5
#define FX_L 6
#define FX_R 8
#define BT_ST 10

// knob keys
#define KNOB_L_CCW 'q'
#define KNOB_L_CW 'w'
#define KNOB_R_CCW 'o'
#define KNOB_R_CW 'p'

#include <Encoder.h>
#include <Keyboard.h>

Encoder enc1(0, 1);
Encoder enc2(2, 3);
float knob1 = 0;
float knob2 = 0;
float old_knob1 = 0;
float old_knob2 = 0;

// knob key state
bool knob1_pressing = false;
bool knob2_pressing = false;
char knob1_key = 0;
char knob2_key = 0;

void setup() {
  pinMode(BT_A, INPUT_PULLUP);
  pinMode(BT_B, INPUT_PULLUP);
  pinMode(BT_C, INPUT_PULLUP);
  pinMode(BT_D, INPUT_PULLUP);
  pinMode(FX_L, INPUT_PULLUP);
  pinMode(FX_R, INPUT_PULLUP);
  pinMode(BT_ST, INPUT_PULLUP);

  Keyboard.begin();
}

void loop() {
  knob1 = (float)(enc1.read());
  knob2 = (float)enc2.read();

  // VOL-L
  if (knob1 != old_knob1) {
    // release prev key
    if (knob1_pressing) {
      Keyboard.release(knob1_key);
      knob1_pressing = false;
    }

    // press new key
    if (knob1 < old_knob1) {
      knob1_key = KNOB_L_CCW;
    } else {
      knob1_key = KNOB_L_CW;
    }
    Keyboard.press(knob1_key);
    knob1_pressing = true;

    if (knob1 < -255 || knob1 > 255) {
      enc1.write(0);
      old_knob1 = 0;
    } else {
      old_knob1 = knob1;
    }
  } else if (knob1_pressing) {
    // knob stopped
    Keyboard.release(knob1_key);
    knob1_pressing = false;
  }

  // VOL-R
  if (knob2 != old_knob2) {
    // release prev key
    if (knob2_pressing) {
      Keyboard.release(knob2_key);
      knob2_pressing = false;
    }

    // press new key
    if (knob2 > old_knob2) {
      knob2_key = KNOB_R_CW;
    } else {
      knob2_key = KNOB_R_CCW;
    }
    Keyboard.press(knob2_key);
    knob2_pressing = true;

    if (knob2 < -255 || knob2 > 255) {
      enc2.write(0);
      old_knob2 = 0;
    } else {
      old_knob2 = knob2;
    }
  } else if (knob2_pressing) {
    // knob stopped
    Keyboard.release(knob2_key);
    knob2_pressing = false;
  }

  // buttons
  if (digitalRead(BT_A) == LOW) {
    Keyboard.press('d');
  } else {
    Keyboard.release('d');
  }
  if (digitalRead(BT_B) == LOW) {
    Keyboard.press('f');
  } else {
    Keyboard.release('f');
  }
  if (digitalRead(BT_C) == LOW) {
    Keyboard.press('j');
  } else {
    Keyboard.release('j');
  }
  if (digitalRead(BT_D) == LOW) {
    Keyboard.press('k');
  } else {
    Keyboard.release('k');
  }
  if (digitalRead(FX_L) == LOW) {
    Keyboard.press('c');
  } else {
    Keyboard.release('c');
  }
  if (digitalRead(FX_R) == LOW) {
    Keyboard.press('m');
  } else {
    Keyboard.release('m');
  }
  if (digitalRead(BT_ST) == LOW) {
    Keyboard.press(KEY_RETURN);
  } else {
    Keyboard.release(KEY_RETURN);
  }

  delay(1);
}
