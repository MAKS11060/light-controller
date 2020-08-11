// connections key
// #define PROTOCOL_KEY 0b10010010
// #define PROTOCOL_KEY 0b01001100
// #define PROTOCOL_KEY 0b00110010
// #define PROTOCOL_KEY 0b00011100

namespace serialParser {
  bool SerialIsData(int16_t &data) {
    return data == -1 ? false : true;
  }
  void SerialAFK() {
    handler_IRL();
    handler_Encoder();

    if (!IRLremote.receiving()) {
      sfx::render();
    }
  }

  struct PROTOCOL {
    uint8_t key[4] = {
      0b10010010, //
      0b01001100,
      0b00110010,
      0b00011100};
    uint16_t start = 0xf00f;
    uint16_t end   = 0xff00;
  };
  const PROTOCOL protocol;


  #define PROTOCOL_TYPE_POWER 1
  #define PROTOCOL_TYPE_PLAY 2
  #define PROTOCOL_TYPE_set_brightness 3
  #define PROTOCOL_TYPE_set_color 4



  int16_t data;
  static uint16_t c = 0;
  static uint16_t payload = 0;
  static uint16_t i = 0;
  static uint8_t buf[1];
  void parse() {
    while(Serial.available() > 0) {data = Serial.read();
      if (SerialIsData(data)) {

        // start
        if (c == 0 && data == (protocol.start >> 8))   {c++;continue;} else
        if (c == 1 && data == (protocol.start & 0xff)) {c++;continue;} else
        // payload lenght
        if (c == 2) {payload = (data << 8);c++;} else
        if (c == 3) {payload |= data;      c++;}
        // c >= 4
        if (c == 4) {c++;
          Serial.write(protocol.key[0]);
          Serial.write(protocol.key[1]);
          Serial.write(protocol.key[2]);
          Serial.write(protocol.key[3]);
          Serial.write(0x0);
        }

        while(payload) {data = Serial.read();
          if (SerialIsData(data)) {payload--;
            // Serial.write(data);

            switch(data) {
              case PROTOCOL_TYPE_PLAY:
                state::opts ^= OPTIONS_PLAY;
                break;
              case PROTOCOL_TYPE_set_brightness:
                i = 1;
                while(i) {data = Serial.read();
                  if (SerialIsData(data)) {i--;payload--;
                    state::brightness = data;
                  }
                }
                break;
              case PROTOCOL_TYPE_set_color:
                while(true) {data = Serial.read(); // leds
                  if (SerialIsData(data)) {payload--;
                    i = data;

                    while(true) {data = Serial.read();
                      if (SerialIsData(data)) {
                        
                      }
                    }

                    break;
                  }
                }
                break;
            }

            if (!payload) Serial.write(0x0);
          }
        }

      }
    }
  }
}



  // #define BRK(DATA) Serial.write(0xff);Serial.write(0x0);
  // #define WRITE(DATA) Serial.write(DATA);
  // #define S_if(T) if (data == T)

  // // level 1
  // #define S_START               (1 << 0) // GLOBAL CONN
  // #define S_MSG                 (1 << 1)
  // #define S_INFO                (1 << 6)
  // #define S_PING                (1 << 7)

  // // level 2
  // #define S_END                 (1 << 0)

  // #define S_MSG__POWER          (1 << 1) // POWER MODE
  // #define S_MSG__PLAY           (1 << 2) // PLAY MODE

  
  // #define S_MSG__BRIGHTNESS     (1 << 3) // Brightness
  // #define S_MSG__BRIGHTNESS_END (1 << 0) // end
  // #define S_MSG__BRIGHTNESS_SET (1 << 1) // set
  // #define S_MSG__BRIGHTNESS_GET (1 << 2) // get


  // #define S_MSG__EFFECT         (1 << 4) // Effect mode


  // int16_t data;
  // static uint8_t c = 0;
  // void parse() {
  //   while(Serial.available() > 0) {data = Serial.read();if (SerialIsData(data)) { // 0

  //     S_if(S_START) { // get token
  //       // data = Serial.read();
  //       Serial.write(S_REMOTE_CONN_KEY_0);
  //       Serial.write(S_REMOTE_CONN_KEY_1);
  //       Serial.write(S_REMOTE_CONN_KEY_2);
  //       Serial.write(S_REMOTE_CONN_KEY_3);
  //     }

  //     S_if(S_MSG) {while(true) {data = Serial.read();
  //       if (SerialIsData(data)) { // 1
  //         if (data == S_END) break;

  //         S_if(S_MSG__POWER) {while(true) {data = Serial.read()
  //           if (SerialIsData(data)) {
  //             if (data == S_END) break;

  //             if (data == 0) state::opts &= ~OPTIONS_POWER; // off
  //             else state::opts |= OPTIONS_POWER; // on
  //           }
  //         }}

  //         S_if(S_MSG__PLAY) {while(true) {data = Serial.read()
  //           if (SerialIsData(data)) {
  //             if (data == S_END) break;

  //             if (data == 0) state::opts &= ~OPTIONS_PLAY; // off
  //             else state::opts |= OPTIONS_PLAY; // on
  //           }
  //         }}

  //         S_if(S_MSG__BRIGHTNESS) {while(true) {data = Serial.read()
  //           if (SerialIsData(data)) {
  //             FastLED.setBrightness(data);
  //             break;
  //           }
  //         }}


  //       }
  //     }}
     
  //     S_if(S_INFO) {

  //       WRITE(0xff)
  //       WRITE(0xff)

  //       WRITE(LEDS_COUNT)
  //       WRITE(MIN_BRIGHTNESS)
  //       WRITE(MAX_BRIGHTNESS)
  //       WRITE(SFX_EFFECTS_COUNT)
  //       WRITE(MAX_FPS)
  //       WRITE(FPS)

  //       WRITE(0xff)
  //       WRITE(0x00)
  //     }
     
  //     S_if(S_PING) {while(true) {data = Serial.read();
  //       if (SerialIsData(data)) {
  //         if (data == S_END) break;

  //         Serial.write(data);

  //       }
  //     }}

  //   }}
  // }

  /*void parse() {
    while(Serial.available() > 0) { // level 0
      data = Serial.read();
      if (SerialIsData(data)) { // level 1
        Serial.println("Level 1");

        if (data & S_START) { // start connections
          // data = Serial.read();
          Serial.write(S_REMOTE_CONN_KEY_0);
          Serial.write(S_REMOTE_CONN_KEY_1);
          Serial.write(S_REMOTE_CONN_KEY_2);
          Serial.write(S_REMOTE_CONN_KEY_3);
        }

        if (data & S_MSG) { // level 2
          while(true) {
            data = Serial.read();
            if (SerialIsData(data)) {
              Serial.println("Level 2");
              if (data & S_END) break;

              if (data & S_MSG__POWER) { // level 3
                while(true) {
                  data = Serial.read();
                  if (SerialIsData(data)) {
                    Serial.println("Level 3");

                    if (data == 0) state::opts &= ~OPTIONS_POWER; // off
                    else state::opts |= OPTIONS_POWER; // on

                    break;
                  } else { // change context

                  }
                }
              }
            }
          }
        }

        if (data & S_PING) {
          while(true) {
            data = Serial.read();
            if (SerialIsData(data)) {
              if (data & S_END) break;

              Serial.write(data); // return data

            } else {
              // other
            }
          }
        }

      } //else { // if not data // change processor context // bad
        // SerialAFK();
      //}
    }
  }*/
