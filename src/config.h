#pragma once

#define SERIAL_SPEED 115200

// Leds setup
#define DATA_PIN 11      // 11 pin (MISO)
#define NUM_LEDS 60      // leds count
#define FPS (1000 / 60)  // 60 fps

// IR setup
#define IR_PIN PD2  // Interrupt pin https://pinouts.ru/images/arduino/mini.png
#define IR_VCC PD4  // +5V for ir receiver

// LDR setup
#define LDR_PIN A1
#define LDR_GND A0
#define LDR_VCC A2

enum IrButton {
  BrightnessUp = 0x5C,
  BrightnessDown = 0x5D,
  Play = 0x41,
  Power = 0x40,

  colorRed = 0x58,
  colorGreen = 0x59,
  colorBlue = 0x45,
  colorWhite = 0x44,

  colorBlazeOrange = 0x54,
  colorLime = 0x55,
  colorAzure = 0x49,
  colorLightPink = 0x48,

  colorOrange = 0x50,
  colorCyan = 0x51,
  colorLapisBlue = 0x4D,
  colorPink = 0x4C,

  colorYellowishOrange = 0x1C,
  colorOcean = 0x1D,
  colorMagenta = 0x1E,
  colorLightBlue = 0x1F,

  colorYellow = 0x18,
  colorTeal = 0x19,
  colorHibiscus = 0x1A,
  colorBluishCyan = 0x1B,

  RedIncress = 0x14,
  GreedIncress = 0x15,
  BlueIncress = 0x16,

  RedDecress = 0x10,
  GreedDecress = 0x11,
  BlueDecress = 0x12,

  Quick = 0x17,
  Slow = 0x13,
  Auto = 0x0F,
  Flash = 0x0B,

  Jump3 = 0x04,
  Jump7 = 0x05,
  Fade3 = 0x06,
  Fade7 = 0x07,

  Diy1 = 0x0C,
  Diy2 = 0x0D,
  Diy3 = 0x0E,
  Diy4 = 0x08,
  Diy5 = 0x09,
  Diy6 = 0x0A,
};
