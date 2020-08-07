#pragma once

/***************************************
  GLOBAL OPTIONS
***************************************/
// Serial port
#define SERIAL_BAUD_RATE 115200

// FastLED
#define PIN_LED 12
#define LEDS_COUNT 60

#define MIN_BRIGHTNESS 2              // 0..250
#define MAX_BRIGHTNESS 255            // 1..255

// Encoder
#define PIN_CLK 5
#define PIN_DT 6
#define PIN_SW 7

// IRLremote
#define PIN_IR 2

/***************************************
  SFX OPTIONS
***************************************/
#define SFX_EFFECTS_COUNT 3           // effects count

#define MAX_FPS 60                    // FPS Limit
#define FPS 30                        // 1..MAX_FPS

/***************************************
  SFX-EFFECTS OPTION
***************************************/
#define SFX_EFFECT_200_LED (LEDS_COUNT / 2)
