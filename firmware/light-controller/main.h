#include <Arduino.h>
#include <FastLED.h>      // link to git
#include <IRLremote.h>    // link to git
#include <GyverEncoder.h> // link to git
#include "options.h"

CRGB    leds[LEDS_COUNT];
Encoder enc1(PIN_CLK, PIN_DT, PIN_SW);
CNec    IRLremote; // use HEX keys code


namespace state {
  /* options */
  #define OPTIONS_POWER     (1 << 0)
  #define OPTIONS_PLAY      (1 << 1)

  #define OPTIONS_MANUAL    (1 << 5)
  #define OPTIONS_REMOTE    (1 << 6)
  #define OPTIONS_CONNECTED (1 << 7)

  uint8_t opts = 0 | OPTIONS_POWER | OPTIONS_PLAY;

  // Encoder
  #define ENCODER_MODE_BRIGHTNESS (1 << 0)
  uint8_t encoder = 0 | ENCODER_MODE_BRIGHTNESS;

  // sfx
  uint8_t fps = (1000 / FPS);
  uint8_t effect = 0;
  uint8_t brightness = 25; // static colors
  struct {
    uint8_t  brightness = 25;
    uint16_t interval   = 250;
  } sfx_effects[SFX_EFFECTS_COUNT];
}

namespace sfx {
  uint32_t timer = 0;
  uint32_t timer_show = 0;

  void r_effects(uint8_t effect);
  void render() {
    if (state::opts & OPTIONS_POWER) { // on
      if (state::opts & OPTIONS_PLAY) { // play
        if (millis() - timer >= state::sfx_effects[state::effect].interval) {
          FastLED.setBrightness(state::sfx_effects[state::effect].brightness);
          timer = millis();
          r_effects(state::effect);
        }
      } else { // pause
        FastLED.setBrightness(state::brightness);
      }
    } else { // off
      r_effects(200);
    }

    if (millis() - timer_show >= state::fps) { // update leds
      timer_show = millis();
      FastLED.show();
    }
  }
}

#include "sfx.h"


namespace method {
  void mode_next() {
    if (++state::effect >= SFX_EFFECTS_COUNT)
      state::effect = 0;

    // Serial.println(state::effect);
  }
  void mode_back() {
    if (--state::effect <= 0)
      state::effect = SFX_EFFECTS_COUNT;
  }
  void mode_set(uint8_t value) {
    // state::opts &= ~(OPTIONS_MANUAL);
    if (value <= SFX_EFFECTS_COUNT)
      state::effect = value;
    else state::effect = SFX_EFFECTS_COUNT;
  }

  void sw_power() {
    state::opts ^= OPTIONS_POWER;
    if (!(state::opts & OPTIONS_PLAY)) sfx::r_effects(state::effect); // fill buffer
  }
  void sw_play() {
    if (state::opts & OPTIONS_POWER) {
      if (state::opts & OPTIONS_PLAY) mode_next();
      else state::opts ^= OPTIONS_PLAY;
    }
  }

  void brightness_increase(uint8_t val) {
    if (state::opts & OPTIONS_PLAY) {
      if (state::sfx_effects[state::effect].brightness + val <= MAX_BRIGHTNESS)
        state::sfx_effects[state::effect].brightness += val;
      else state::sfx_effects[state::effect].brightness = MAX_BRIGHTNESS;
    } else {
      if (state::brightness + val <= MAX_BRIGHTNESS)
        state::brightness += val;
      else state::brightness = MAX_BRIGHTNESS;
    }
  }
  void brightness_decrease(uint8_t val) {
    if (state::opts & OPTIONS_PLAY) {
      if (state::sfx_effects[state::effect].brightness - val >= MIN_BRIGHTNESS)
        state::sfx_effects[state::effect].brightness -= val;
      else state::sfx_effects[state::effect].brightness = MIN_BRIGHTNESS;
    } else {
      if (state::brightness - val >= MIN_BRIGHTNESS)
        state::brightness -= val;
      else state::brightness = MIN_BRIGHTNESS;
    }
  }

  void interval_increase(uint16_t val) {
    if (state::opts & OPTIONS_PLAY) {
      if (state::sfx_effects[state::effect].interval + val <= 1e4)
        state::sfx_effects[state::effect].interval += val;
      else state::sfx_effects[state::effect].interval = 1e4;
    }
  }
  void interval_decrease(uint16_t val) {
    if (state::opts & OPTIONS_PLAY) {
      if ((int)state::sfx_effects[state::effect].interval - (int)val >= 0)
        state::sfx_effects[state::effect].interval -= val;
      else state::sfx_effects[state::effect].interval = 0;
    }
  }

  void color_fill(uint32_t color) {
    state::opts &= ~OPTIONS_PLAY;

    for (int i = 0; i < LEDS_COUNT; i++)
      leds[i] = color;
  }
  void set_fps(uint8_t value) {
    if (value < MAX_FPS) state::fps = value;
    else state::fps = 1000 / MAX_FPS;
  }

  void addColor(uint8_t r, uint8_t g, uint8_t b) {
    state::opts &= ~OPTIONS_PLAY;
    for (int i = 0; i < LEDS_COUNT; i++)
      leds[i] += CRGB(r, g, b);
  }
  void subColor(uint8_t r, uint8_t g, uint8_t b) {
    state::opts &= ~OPTIONS_PLAY;
    for (int i = 0; i < LEDS_COUNT; i++)
      leds[i] -= CRGB(r, g, b);
  }
}


void setup() {
  pinMode(13, OUTPUT);

  // Serial port
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("Serial enabled");
  Serial.write(0x3);

  // Leds
  FastLED.addLeds<WS2811, PIN_LED, GRB>(leds, LEDS_COUNT)
    .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(15);

  // IR
  pinMode(3, OUTPUT); digitalWrite(3, HIGH); //IR +5v
  if (!IRLremote.begin(PIN_IR)) Serial.println(F("You did not choose a valid pin."));
}


// Encoder
void handler_Encoder() {
  enc1.tick();

  if (state::opts & OPTIONS_REMOTE) {
    // send encoder actions to serial port
    return;
  }


  if (enc1.isRight()) {// ==>
    if (state::encoder & ENCODER_MODE_BRIGHTNESS) {
      method::brightness_increase(20);
    } else {
      method::interval_decrease(25);
    }
  }

  if (enc1.isLeft()) {// <==
   if (state::encoder & ENCODER_MODE_BRIGHTNESS) {
      method::brightness_decrease(10);
    } else {
      method::interval_increase(50);
    }
  }

  if (enc1.isPress()) {// bad code
    state::encoder ^= ENCODER_MODE_BRIGHTNESS;
  }


  
  // if (enc1.isTurn()) {}
  // if (enc1.isRight()) {// ==>
  /*  if (encoder_state & ) {// if selected brightness mode

    } else {

    }*/
  // }
  // if (enc1.isLeft()) {// <==
    // if (true) {// if selected brightness mode
  
    // }
  // }

  // if (enc1.isRightH()) {}
  // if (enc1.isLeftH()) {}

  // if (enc1.isFastR()) {}
  // if (enc1.isFastL()) {}

  // if (enc1.isPress()) {}
  // if (enc1.isRelease()) {}
  // if (enc1.isHolded()) {}
}

// TTL UART
void handler_Serial() {
  int16_t data;

  while(Serial.available() > 0) {
    data = Serial.read();

    Serial.write(data);
  }
}

// IRLremote
void handler_IRL_keys(uint8_t &command, uint8_t &holded) {

  /* Brightness */
  #define BTN_LIGHTER            0x5C
  #define BTN_DARKEN             0x5D

  /* State */
  #define BTN_PLAY               0x41
  #define BTN_POWER              0x40

  /* Color keys */
  #define BTN_COLOR_red          0x58
  #define BTN_COLOR_orange       0x54
  #define BTN_COLOR_dark_yellow  0x50
  #define BTN_COLOR_yellow       0x1C
  #define BTN_COLOR_light_yellow 0x18

  #define BTN_COLOR_green        0x59
  #define BTN_COLOR_light_green  0x55
  #define BTN_COLOR_cyan         0x51
  #define BTN_COLOR_light_blue   0x1D
  #define BTN_COLOR_sky_blue     0x19

  #define BTN_COLOR_blue         0x45
  #define BTN_COLOR_dark_blue    0x49
  #define BTN_COLOR_lyons_blue   0x4D
  #define BTN_COLOR_purple       0x1E
  #define BTN_COLOR_brown        0x1A

  #define BTN_COLOR_white        0x44
  #define BTN_COLOR_milk_white   0x48
  #define BTN_COLOR_white_pink   0x4C
  #define BTN_COLOR_green_white  0x1F
  #define BTN_COLOR_blue_white   0x1B

  /* Color channels */
  #define BTN_increase_red       0x14
  #define BTN_increase_green     0x15
  #define BTN_increase_blue      0x16
  #define BTN_decrease_red       0x10
  #define BTN_decrease_green     0x11
  #define BTN_decrease_blue      0x12

  /* Speed */
  #define BTN_QUICK              0x17
  #define BTN_SLOW               0x13

  /* DIY keys */
  #define BTN_DIY_KEY1           0x0C
  #define BTN_DIY_KEY2           0x0D
  #define BTN_DIY_KEY3           0x0E
  #define BTN_DIY_KEY4           0x08
  #define BTN_DIY_KEY5           0x09
  #define BTN_DIY_KEY6           0x0A

  /* Effects */
  #define BTN_JUMP3              0x04
  #define BTN_JUMP7              0x05
  #define BTN_FADE3              0x06
  #define BTN_FADE7              0x07
  #define BTN_FLASH              0x0B
  #define BTN_AUTO               0x0F

  uint16_t &interval = state::sfx_effects[state::effect].interval;
  uint8_t  &brightness = state::sfx_effects[state::effect].brightness;

  // Serial.print("IRL>");
  // Serial.print(" C 0x");Serial.print(command, HEX);
  // Serial.print(" Holded: ");Serial.print(holded);
  // Serial.print(" | B: ");Serial.print(brightness);
  // Serial.print(" - I: ");Serial.println(interval);

  if (holded > 0) { // is holding button
    switch(command) {
      case BTN_LIGHTER: method::brightness_increase(5);break;
      case BTN_DARKEN:  method::brightness_decrease(5);break;
      case BTN_QUICK:   method::interval_decrease(5);break;
      case BTN_SLOW:    method::interval_increase(5);break;

      case BTN_increase_red:   method::addColor(5, 0, 0);break;
      case BTN_increase_green: method::addColor(0, 5, 0);break;
      case BTN_increase_blue:  method::addColor(0, 0, 5);break;
      case BTN_decrease_red:   method::subColor(5, 0, 0);break;
      case BTN_decrease_green: method::subColor(0, 5, 0);break;
      case BTN_decrease_blue:  method::subColor(0, 0, 5);break;
    }
    return;
  }

  switch(command) {

    /* Brightness */
    case BTN_LIGHTER:
      if      (brightness > 150)  method::brightness_increase(10);
      else if (brightness > 15)   method::brightness_increase(5);
      else if (brightness > 0)    method::brightness_increase(1);
      break;
    case BTN_DARKEN:
      if     (brightness >= 150)  method::brightness_decrease(10);
      else if (brightness >= 15)  method::brightness_decrease(5);
      else                        method::brightness_decrease(1);
      break;

    /* State */
    case BTN_PLAY:              method::sw_play();break;
    case BTN_POWER:             method::sw_power();break;

    /* Color keys */
    case BTN_COLOR_red:         method::color_fill(0xff0000);break;
    case BTN_COLOR_orange:      method::color_fill(0xffa500);break;
    case BTN_COLOR_dark_yellow: method::color_fill(0x000000);break;
    case BTN_COLOR_yellow:      method::color_fill(0xffff00);break;
    case BTN_COLOR_light_yellow:method::color_fill(0x000000);break;

    case BTN_COLOR_green:       method::color_fill(0x00ff00);break;
    case BTN_COLOR_light_green: method::color_fill(0x90ee90);break;
    case BTN_COLOR_cyan:        method::color_fill(0x00ffff);break;
    case BTN_COLOR_light_blue:  method::color_fill(0x000000);break;
    case BTN_COLOR_sky_blue:    method::color_fill(0x000000);break;

    case BTN_COLOR_blue:        method::color_fill(0x0000ff);break;
    case BTN_COLOR_dark_blue:   method::color_fill(0x000000);break;
    case BTN_COLOR_lyons_blue:  method::color_fill(0x000000);break;
    case BTN_COLOR_purple:      method::color_fill(0x000000);break;
    case BTN_COLOR_brown:       method::color_fill(0x000000);break;

    case BTN_COLOR_white:       method::color_fill(0xffffff);break;
    case BTN_COLOR_milk_white:  method::color_fill(0x000000);break;
    case BTN_COLOR_white_pink:  method::color_fill(0x000000);break;
    case BTN_COLOR_green_white: method::color_fill(0x000000);break;
    case BTN_COLOR_blue_white:  method::color_fill(0x000000);break;

    /* Color channels */
    case BTN_increase_red:      method::addColor(5, 0, 0);break;
    case BTN_increase_green:    method::addColor(0, 5, 0);break;
    case BTN_increase_blue:     method::addColor(0, 0, 5);break;
    case BTN_decrease_red:      method::subColor(5, 0, 0);break;
    case BTN_decrease_green:    method::subColor(0, 5, 0);break;
    case BTN_decrease_blue:     method::subColor(0, 0, 5);break;

    /* Speed */
    case BTN_QUICK:
      if (interval > 500)       method::interval_decrease(250);
      else if (interval > 150)  method::interval_decrease(50);
      else if (interval > 50)   method::interval_decrease(10);
      else if (interval > 20)   method::interval_decrease(5);
      else if (interval > 6)    method::interval_decrease(2);
      else if (interval > 0)    method::interval_decrease(1);
      break;
    case BTN_SLOW:
      if (interval >= 500)      method::interval_increase(250);
      else if (interval >= 150) method::interval_increase(50);
      else if (interval >= 50)  method::interval_increase(10);
      else if (interval >= 20)  method::interval_increase(5);
      else if (interval >= 6)   method::interval_increase(2);
      else                      method::interval_increase(1);
      break;

    /* DIY keys */
    case BTN_DIY_KEY1:
    case BTN_DIY_KEY2:
    case BTN_DIY_KEY3:
    case BTN_DIY_KEY4:
    case BTN_DIY_KEY5:
    case BTN_DIY_KEY6:

    /* Effects */
    case BTN_JUMP3:
    case BTN_JUMP7:
    case BTN_FADE3:
    case BTN_FADE7:
    case BTN_FLASH:
    case BTN_AUTO: break;
  }
}

void handler_IRL() { // IR-remote 44 key
  if (IRLremote.available()) {
    // Get the new data from the remote
    auto data = IRLremote.read();
    // Print the protocol data
    // Serial.print(F("Address: 0x")); Serial.println(data.address, HEX);
    // Serial.print(F("Command: 0x")); Serial.println(data.command, HEX);

    if (state::opts & OPTIONS_REMOTE) {
      // send IRLremote actions to serial port
      return;
    }


    static uint8_t lastCommand = 0;
    static uint8_t holding_counter = 0;
    if (data.command != 0x0) {
      holding_counter = 0;
      lastCommand = data.command;
      handler_IRL_keys(data.command, holding_counter);
    } else if (data.address == 0xffff && data.command == 0 && lastCommand != 0) {
      if (++holding_counter > 255) holding_counter = 1;
      // bounce protection
      if (holding_counter > 3) handler_IRL_keys(lastCommand, holding_counter);

      // Serial.print(holding_counter);
      // Serial.print(" holding: 0x");
      // Serial.println(lastCommand, HEX);
    }

    // Serial.print(state::dynamic_effect);
    // Serial.print("-");
    // Serial.print(state::sfx_effects[state::effect].brightness);
    // Serial.print("-");
    // Serial.println(state::sfx_effects[state::effect].interval);
  }
}



void loop() {
  handler_Encoder();
  handler_Serial();
  handler_IRL();

  if (!IRLremote.receiving()) {
    sfx::render();
  }
}
