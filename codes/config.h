/*
 * Pin Configuration
 * Modify these values to match your hardware setup
 */

#ifndef CONFIG_H
#define CONFIG_H

// ========================================
// BUTTON PINS
// ========================================
#define BUT_PIN_START 10
#define BUT_PIN_A 11
#define BUT_PIN_B 9
#define BUT_PIN_C 7
#define BUT_PIN_D 5
#define BUT_PIN_FXL 6
#define BUT_PIN_FXR 8

// ========================================
// ENCODER PINS (Rotary Encoders)
// ========================================
#define ENC_L_A 0 // Left encoder pin A
#define ENC_L_B 1 // Left encoder pin B
#define ENC_R_A 2 // Right encoder pin A
#define ENC_R_B 3 // Right encoder pin B

// ========================================
// SETTINGS
// ========================================
#define REPORT_DELAY 1  // HID report delay in milliseconds
#define MILLIDEBOUNCE 5 // Button debounce time in milliseconds
#define KONAMI_SPOOF                                                           \
  1 // Enable Konami VID/PID spoofing (1 = enabled, 0 = disabled)
#define KNOB_DEADZONE 0 // Encoder deadzone (0 = maximum sensitivity)

#endif
