#include <Arduino.h>
#include <FastLED.h>
#include <IRLremote.h>

#include "utils.h"
#include "config.h"

#define LED_FX_COUNT 2
// Flags
#define LED_FLAG_AUTO 0
#define LED_AUTO_BRIGHTNESS_MIN 2

class Led {
 public:
  // FX
  struct {
    uint32_t color = 0xffffff;
    uint8_t brightness = 25;
  } fx_static;
  struct {
    uint16_t interval = 250;
    uint8_t brightness = 50;
  } fx_state[LED_FX_COUNT];
  uint8_t fx = 0;

  // Global
  enum MODE {
    Off,
    Static,
    PlayFX,
  };
  uint8_t mode = MODE::PlayFX;             // Default mode
  uint8_t flags = 0 | bit(LED_FLAG_AUTO);  // Submode
  CRGB leds[NUM_LEDS];                     // Define the array of leds

  // fx vars
  const uint8_t _fx_TOP_INDEX =
      (NUM_LEDS % 2 ? (NUM_LEDS / 2) + 1 : NUM_LEDS / 2);
  uint8_t _fx_hue = 0;
  float _fx_index = (float)255 / (NUM_LEDS / 2);

  void tick() {
    tickRender();

    if (mode == MODE::PlayFX) {
      static uint32_t timer = 0;
      if (millis() - timer >= fx_state[fx].interval) {
        timer = millis();
        tickFX();
      }
    }
  }

  inline void tickRender() {
    static uint32_t timer = 0;
    if (millis() - timer >= FPS) {  // show with fps limit
      timer = millis();
      FastLED.show();
    }
  }

  inline void tickFX() {
    switch (fx) {
      case 0:
        _fx_hue++;
        for (int i = 0; i < NUM_LEDS; i++) {
          // [1,2,3] ==>
          leds[i] = CHSV((i * _fx_index) / 2 - _fx_hue, 255, 255);
        }
        break;

      case 1:
        _fx_hue++;
        for (int i = 0; i < _fx_TOP_INDEX; i++) {
          // [1,2,3,3,2,1] <==>
          leds[_fx_TOP_INDEX - i - 1] = leds[_fx_TOP_INDEX + i] =
              CHSV((i * _fx_index) / 2 - _fx_hue, 255, 255);
        }
        break;
    }
  }

  void toggleMode(MODE val) {
    switch (val) {
      case MODE::Off:
        if (mode == MODE::Off) {
          setColor(0);
          return;
        }

        FastLED.clear();
        FastLED.setBrightness(3);
        leds[NUM_LEDS / 2].r = 255;
        break;
      case MODE::Static:
        FastLED.setBrightness(fx_static.brightness);
        break;
      case MODE::PlayFX:
        if (mode == MODE::PlayFX) {
          if (++fx > LED_FX_COUNT - 1) fx = 0;  // loop fx
        }
        FastLED.setBrightness(fx_state[fx].brightness);
        break;
    }

    mode = static_cast<uint8_t>(val);
  }

  void setBrigthness(uint8_t val) {
    if (mode == MODE::Off) return;  // prevent change

    uint8_t *brightness = &fx_static.brightness;
    if (mode == MODE::PlayFX) brightness = &fx_state[fx].brightness;

    *brightness = val;
    FastLED.setBrightness(*brightness);  // update
  }

  void changeBrightness(bool subtract = false) {
    if (mode == MODE::Off) return;      // prevent change
    if (bitRead(flags, LED_FLAG_AUTO))  // disable auto
      bitClear(flags, LED_FLAG_AUTO);

    auto *brightness = &fx_static.brightness;
    if (mode == MODE::PlayFX) brightness = &fx_state[fx].brightness;

    uint8_t a = *brightness >= 100  ? 20
                : *brightness >= 50 ? 10
                : *brightness >= 20 ? 5
                : *brightness >= 10 ? 2
                                    : 1;

    *brightness = constrain((*brightness) + a * (subtract ? -1 : 1),
                            LED_AUTO_BRIGHTNESS_MIN, 255);
    FastLED.setBrightness(*brightness);  // update
  }

  void changeSpeed(bool subtract = false) {
    if (mode != MODE::PlayFX) return;  // prevent change
    auto interval = &fx_state[fx].interval;
    uint16_t a = (*interval >= 500   ? 250
                  : *interval >= 150 ? 50
                  : *interval >= 50  ? 25
                  : *interval >= 20  ? 5
                  : *interval >= 10  ? 2
                                     : 1);

    *interval = constrain((*interval) + a * (subtract ? -1 : 1), 2, UINT16_MAX);
  }

  void setColor(uint32_t color) {
    if (mode != MODE::Static) {  // use previous color
      toggleMode(MODE::Static);
      setColor(0);
      return;
    }

    if (color != 0) fx_static.color = color;  // update color

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = fx_static.color;
    }
  }

  void changeColor(uint32_t color, bool subtract = false) {
    for (int i = 0; i < NUM_LEDS; i++) {
      if (subtract)
        leds[i] -= CRGB(color);  // subtract
      else
        leds[i] += CRGB(color);  // add
    }
  }
};

CNec ir;  // Define IR handler
Led led;

uint8_t ldrRead() { return (uint8_t)map(analogRead(LDR_PIN), 0, 1023, 0, 255); }

void ldrHanlder() {
  static uint32_t timer = 0;
  if (bitRead(led.flags, LED_FLAG_AUTO) && millis() - timer >= 1000) {
    timer = millis();
    led.setBrigthness(constrain(255 - ldrRead(), 0, 200));  // min max
  }
}

void irHandler(uint8_t c) {
  switch (c) {
    case IrButton::Power:
      led.toggleMode(Led::MODE::Off);
      break;
    case IrButton::Play:
      led.toggleMode(Led::MODE::PlayFX);
      break;
    case IrButton::BrightnessUp:
      led.changeBrightness();
      break;
    case IrButton::BrightnessDown:
      led.changeBrightness(true);
      break;
    case IrButton::Quick:
      led.changeSpeed(true);
      break;
    case IrButton::Slow:
      led.changeSpeed(false);
      break;

    case IrButton::RedIncress:
      led.changeColor(0x100000);
      break;
    case IrButton::GreedIncress:
      led.changeColor(0x001000);
      break;
    case IrButton::BlueIncress:
      led.changeColor(0x000010);
      break;
    case IrButton::RedDecress:
      led.changeColor(0x100000, 1);
      break;
    case IrButton::GreedDecress:
      led.changeColor(0x001000, 1);
      break;
    case IrButton::BlueDecress:
      led.changeColor(0x000010, 1);
      break;

    case IrButton::Auto:
      bitToggle(led.flags, LED_FLAG_AUTO);
      break;
    case IrButton::Flash:
      break;
    case IrButton::Fade3:
      break;
    case IrButton::Fade7:
      break;
    case IrButton::Jump3:
      break;
    case IrButton::Jump7:
      break;

    case IrButton::Diy1:
    case IrButton::Diy2:
    case IrButton::Diy3:
    case IrButton::Diy4:
    case IrButton::Diy5:
    case IrButton::Diy6:
      break;

      // Color picker https://colors.artyclick.com/color-name-finder/
      // https://www.sign-in-global.com/data/productcate/2022-05-06/level68480_1651818001.jpg
    case IrButton::colorRed:
      led.setColor(0xff0000);
      break;
    case IrButton::colorGreen:
      led.setColor(0x00ff00);
      break;
    case IrButton::colorBlue:
      led.setColor(0x0000ff);
      break;
    case IrButton::colorWhite:
      led.setColor(0xffffff);
      break;
    case IrButton::colorBlazeOrange:
      led.setColor(0xff7f00);
      break;
    case IrButton::colorLime:
      led.setColor(0x52d017);
      break;
    case IrButton::colorAzure:
      led.setColor(0x069af3);
      break;
    case IrButton::colorLightPink:
      led.setColor(0xffc6d5);
      break;
    case IrButton::colorOrange:
      led.setColor(0xff6600);
      break;
    case IrButton::colorCyan:
      led.setColor(0x08e8de);
      break;
    case IrButton::colorLapisBlue:
      led.setColor(0x15317e);
      break;
    case IrButton::colorPink:
      led.setColor(0xfec6d1);
      break;
    case IrButton::colorYellowishOrange:
      led.setColor(0xff9f00);
      break;
    case IrButton::colorOcean:
      led.setColor(0x017b92);
      break;
    case IrButton::colorMagenta:
      led.setColor(0x591d35);
      break;
    case IrButton::colorLightBlue:
      led.setColor(0xa3e3ed);
      break;
    case IrButton::colorYellow:
      led.setColor(0xffdf00);
      break;
    case IrButton::colorTeal:
      led.setColor(0x015482);
      break;
    case IrButton::colorHibiscus:
      led.setColor(0xb6316c);
      break;
    case IrButton::colorBluishCyan:
      led.setColor(0x8eebec);
      break;
  }

#ifdef SERIAL_SPEED
  Serial.print(F("M 0b"));
  Serial.print(led.mode, BIN);
  Serial.print(F(" B "));
  Serial.print(FastLED.getBrightness());
  Serial.print(F(" L "));
  Serial.print(constrain(255 - ldrRead(), 0, 200));
  Serial.print(F(" ir 0x"));
  Serial.print(c, HEX);
  Serial.println();
#endif
}

void irHanlderTick() {
  if (ir.available()) {
    auto c = ir.read();
    static uint8_t last_command = 0;
    static uint8_t counter = 0;

    // command handler
    if (c.command != 0x0) {
      last_command = c.command;  // save command when holding button
      counter = 0;               // reset
      irHandler(c.command);
    } else if (c.address == 0xffff && c.command == 0 && last_command != 0) {
      if (++counter > 255) counter = 0;
      if (counter > 5) irHandler(last_command);  // hold
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // led off

  // Init LED
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(led.leds, NUM_LEDS)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(20);

  // Init LDR
  pinMode(LDR_PIN, INPUT);
  pinMode(LDR_GND, OUTPUT);
  pinMode(LDR_VCC, OUTPUT);
  digitalWrite(LDR_GND, LOW);
  digitalWrite(LDR_VCC, HIGH);

  // Init IR
  pinMode(IR_VCC, OUTPUT);
  digitalWrite(IR_VCC, HIGH);
  // attachInterrupt(
  // IR_PIN, []() { ir.tick(); }, FALLING);
  ir.begin(IR_PIN);

  // led.setBrightness(255 - ldrRead());  // init bright

#ifdef SERIAL_SPEED
  Serial.begin(SERIAL_SPEED);
  Serial.print(F("Serial begin "));
  Serial.println(SERIAL_SPEED);
  Serial.print(F("Build "));
  Serial.println(F(__TIMESTAMP__));
  Serial.print(F("Light "));
  Serial.println(ldrRead());
#endif
}

void loop() {
  irHanlderTick();
  ldrHanlder();
  if (!ir.receiving()) led.tick();
}
