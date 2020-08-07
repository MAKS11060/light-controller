const uint8_t Ld2 = (LEDS_COUNT % 2 ? (LEDS_COUNT / 2) + 1 : LEDS_COUNT / 2);

namespace sfx {
  uint8_t hue   = 0;
  uint8_t count = 0;
  float   index = (float)255 / (LEDS_COUNT / 2);

}

void sfx::r_effects(uint8_t effect) {
  switch(effect) {
    case 0:
      count = 0;
      if (++hue > 255) hue = 0;
      // if (hue-- < 0) hue = 255;
      for (int i = 0; i < LEDS_COUNT; i++) {
        // left ==> right
        leds[i] = CHSV(
          (count * index) / 2 - hue, 255, 255
        );
        count++;
      }
      break;

    case 1:
      count = 0;
      if (++hue > 255) hue = 0;
      // if (hue-- < 0) hue = 255;
      for (int i = 0; i < LEDS_COUNT / 2; i++) {
        leds[i] = leds[LEDS_COUNT - i - 1] = CHSV(
          (count * index) / 2 - hue, 255, 255
        );
        count++;
      }
      break;

    case 2:
      count = 0;
      if (++hue > 255) hue = 0;
      // if (hue-- < 0) hue = 255;

      for (int i = 0; i < LEDS_COUNT / 2; i++) {
        leds[Ld2 - i - 1] = leds[i + Ld2] = CHSV(
          (count * index) / 2 - hue, 255, 255
        );
        count++;
      }
      break;


    case 200: // offline
      FastLED.clear();
      FastLED.setBrightness(3);
      leds[SFX_EFFECT_200_LED].red = 255;
      break;
  }
}