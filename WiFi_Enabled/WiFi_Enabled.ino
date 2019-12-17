#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "X1-Extreme";
const char* password= "rOZuHl66";

WiFiUDP udp;

const uint8_t r_pin = 21;
const uint8_t g_pin = 22;
const uint8_t b_pin = 19;
const uint8_t big_r_pin = 18;
const uint8_t big_g_pin = 5;
const uint8_t big_b_pin = 23;
const uint8_t big_ground_pin = 10;
const uint8_t s_pin = 9;
const uint8_t LED_PINS[] = {0, 4, 2, 15, 13, 12, 14, 27, 33, 32};
const uint8_t num_PWM = 7;

uint8_t next_free_channel = 0;
uint8_t prob_LED = 1;
uint8_t prob_SPEAK = 1;
uint8_t prob_BASE = 3;
uint16_t RGB_Colours[] = {0, 255, 0};
uint16_t MAIN_Colours[] = {255, 0, 0};
uint8_t RGB_dec = 1;
uint8_t MAIN_dec = 0;
uint16_t val = 0;

boolean STOP = false;
boolean SILENCE = false;

class RGB_LED {
  private:
    int channel;
    int PWM;
    boolean up;
  
  public:
    boolean switched_on;
  
    RGB_LED() {
      this->channel = 0;
      this->PWM = 256;
      this->switched_on = false;
      this->up = false;
    }

    void set_me(int pin) {
      this->channel = next_free_channel++;
      ledcAttachPin(pin, this->channel);
      ledcSetup(this->channel, 12000, 8);
      ledcWrite(this->channel, this->PWM);
      Serial.print("I am pin ");
      Serial.print(pin);
      Serial.print(" with channel: ");
      Serial.println(this->channel);
    }
  
    void start_me() {
      this->switched_on = true;
      this->up = true;
    }
  
    void run_me() {
      if (this->switched_on) {
        if (this->up) {
          this->PWM -= random(1, 4);
        } else {
          this->PWM += random(1, 4);
        }
    
        if (this->PWM <= 0) {
          this->up = false;
          this->PWM = 0;
        } else if (this->PWM >= 256) {
          this->switched_on = false;
          this->PWM = 256;
        }
    
        ledcWrite(this->channel, this->PWM);
      }
    }

    void test_me() {
      ledcWrite(this->channel, 0);
      delay(200);
      ledcWrite(this->channel, 256);
    }

    void turn_off() {
      ledcWrite(this->channel, 256);
    }

    void turn_on() {
      ledcWrite(this->channel, 0);
    }

    void dim(uint16_t pwm) {
      Serial.print("PWM: ");
      Serial.println(pwm);
      ledcWrite(this->channel, pwm);
    }
};

class BORING_LED {
  private:
    unsigned long end_time;
    int pin;
  public:
    boolean switched_on;

    BORING_LED() {
      switched_on = false;
      end_time = 0;
      pin = 0;
    }

    void set_me(int p) {
      pin = p;
      pinMode(pin, OUTPUT);
      digitalWrite(pin, HIGH);
    }

    void start_me() {
      end_time = millis() + random(600, 1000);
      switched_on = true;
      digitalWrite(pin, LOW);
    }

    void run_me() {
      if (millis() > end_time) {
        switched_on = false;
        digitalWrite(pin, HIGH);
      }
    }

    void test_me() {
      digitalWrite(pin, LOW);
      delay(200);
      if (!switched_on) {
        digitalWrite(pin, HIGH);
      }
    }

    void turn_off() {
      digitalWrite(pin, HIGH);
    }
};

class SPEAKER {
  private: 
    uint8_t channel;
    uint32_t freq;
    unsigned long end_time;
  public:
    boolean switched_on;

    SPEAKER() {
      channel = 0;
      freq = 3000;
      switched_on = false;
    }

    void set_me() {
      channel = next_free_channel++;
      ledcAttachPin(s_pin, channel);
      ledcSetup(channel, 12000, 8);
      Serial.print("I am pin ");
      Serial.print(s_pin);
      Serial.print(" with channel: ");
      Serial.println(this->channel);
    }

    void start_me() {
      switched_on = true;
      ledcWriteTone(channel, freq);
      end_time = millis() + random(200, 400);
    }

    void run_me() {
      if (millis() > end_time) {
        switched_on = false;
        ledcWriteTone(channel, 0);
        freq = 3000;
      } else {
        freq += random(-100, 100);
        ledcWriteTone(channel, freq);
      }
    }

  void test_me() {
    ledcWriteTone(channel, 2000);
    delay(200);
    ledcWriteTone(channel, 0);
  }

  void turn_off() {
    ledcWriteTone(channel, 0);
  }

  void play_freq(uint32_t f) {
    ledcWriteTone(channel, f);
  }
};

RGB_LED lamp;
RGB_LED lights[num_PWM];
BORING_LED no_PWM_lights[10 - num_PWM];
SPEAKER speak;

void setup() {
  Serial.begin(115200);
  ledcAttachPin(r_pin, next_free_channel);
  ledcSetup(next_free_channel++, 12000, 8);
  ledcAttachPin(g_pin, next_free_channel);
  ledcSetup(next_free_channel++, 12000, 8);
  ledcAttachPin(b_pin, next_free_channel);
  ledcSetup(next_free_channel++, 12000, 8);
  ledcAttachPin(big_r_pin, next_free_channel);
  ledcSetup(next_free_channel++, 12000, 8);
  ledcAttachPin(big_g_pin, next_free_channel);
  ledcSetup(next_free_channel++, 12000, 8);
  ledcAttachPin(big_b_pin, next_free_channel);
  ledcSetup(next_free_channel++, 12000, 8);

  lamp.set_me(big_ground_pin);
  
  for (int i = 0; i < num_PWM; i++) {
    lights[i].set_me(LED_PINS[i]);
  }

  for (int i = num_PWM; i < 10; i++) {
    no_PWM_lights[i - num_PWM].set_me(LED_PINS[i]);
  }

  // Speaker setup
  speak.set_me();

  ConnectToWiFi();
}

void loop() {
  if (STOP) {
    checkDead();
    delay(100);
    return;
  }
  if (val >= 255) {
    RGB_dec++;
    RGB_dec = RGB_dec % 3;
    MAIN_dec++;
    MAIN_dec = MAIN_dec % 3;
    val = 0;
  }
  uint8_t RGB_inc = RGB_dec == 2 ? 0 : RGB_dec + 1;
  RGB_Colours[RGB_dec]--;
  RGB_Colours[RGB_inc]++;
  uint8_t MAIN_inc = MAIN_dec == 2 ? 0 : MAIN_dec + 1;
  MAIN_Colours[MAIN_dec]--;
  MAIN_Colours[MAIN_inc]++;
  RGB_Set(RGB_Colours[0], RGB_Colours[1], RGB_Colours[2]);
  MAIN_Set(MAIN_Colours[0], MAIN_Colours[1], MAIN_Colours[2]);
  val++;

  for (int i = 0; i < num_PWM; i++) {
    if (lights[i].switched_on) {
      lights[i].run_me();
    } else if (random(0, 1001) < prob_LED) {
      lights[i].start_me();
    }
  }

  for (int i = 0; i < 10 - num_PWM; i++) {
    if (no_PWM_lights[i].switched_on) {
      no_PWM_lights[i].run_me();
    } else if (random(0, 1001) < prob_LED) {
      no_PWM_lights[i].start_me();
    }
  }

  if (lamp.switched_on) {
    lamp.run_me();
  } else if (random(0, 1001) < prob_BASE) {
    lamp.start_me();
  }

  if (!SILENCE && speak.switched_on) {
    speak.run_me();
  } else if (!SILENCE && random(0, 1001) < prob_SPEAK) {
    speak.start_me();
  }
  checkDead();
  delay(10);
}

void manual_override() {
  boolean automatic = false;
  boolean turned_on[12] = {false};
  while (!automatic) {
    uint8_t buffer[50] = "";
    memset(buffer, 0, 50);
    udp.parsePacket();
    String tmp;
    
    if(udp.read(buffer, 50) > 0){
      tmp = String((char *) buffer);
      Serial.println(tmp);
      if (tmp.startsWith("Auto")) {
        Serial.println("CMD: AUTO");
        automatic = true;
      } else if (tmp.startsWith("LED")) {
        int val = (int) tmp[4] - (int) 'A';
        if (val < 11) {
          Serial.print("CMD: LED ");
          Serial.print(val);
          if (val < num_PWM) {
            if (turned_on[val]) {   
              lights[val].turn_off();
              Serial.println(" OFF");
              turned_on[val] = false;
            } else {
              lights[val].turn_on();
              Serial.println(" ON");
              turned_on[val] = true;
            }
          } else if (val < 10) {
            if (turned_on[val]) {   
              no_PWM_lights[val - num_PWM].turn_off();
              Serial.println(" OFF");
              turned_on[val] = false;
            } else {
              no_PWM_lights[val - num_PWM].start_me();
              Serial.println(" ON");
              turned_on[val] = true;
            }
          } else {
            if (turned_on[val]) {   
              lamp.turn_off();
              Serial.println(" OFF");
              turned_on[val] = false;
            } else {
              lamp.turn_on();
              Serial.println(" ON");
              turned_on[val] = true;
            }
          }
        }
      } else if (tmp.startsWith("Dim")) {
        int val = (int) tmp[4] - (int) 'A';
        Serial.println(val);
        int dval = 100 * (tmp[6] - '0') + 10 * (tmp[7] - '0') + (tmp[8] - '0');
        if (val < 11) {
          Serial.print("CMD: LED ");
          Serial.print(val);
          Serial.print(" DIMMED TO ");
          Serial.println(dval);
          if (val < num_PWM) {
            if (turned_on[val]) {
              lights[val].dim(dval);
            }
          } else if (val < 10) {
            Serial.println("Invalid LED choice. This does not support PWM!");
          } else {
            if (turned_on[10]) {
              lamp.dim(dval);
            }
            Serial.print("CMD: MAIN DIMMED TO ");
            Serial.println(dval);
          }
        }
      } else if (tmp.startsWith("Speaker")) {
        Serial.println("CMD: SPEAKER");
        if (turned_on[11]) {
          speak.turn_off();
          turned_on[11] = false;
        } else {
          speak.start_me();
          turned_on[11] = true;
        }
      } else if (tmp.startsWith("s")) {
        Serial.print("CMD: CHANGE FREQUENCY TO ");
        uint32_t val = 1000 * (tmp[2] - '0') + 100 * (tmp[3] - '0') + 10 * (tmp[4] - '0') + (tmp[5] - '0');
        Serial.println(val);
        if (turned_on[11]) {
          speak.play_freq(val);
        }
      } else if (tmp.startsWith("c")) {
        uint16_t r = 100 * (tmp[2] - '0') + 10 * (tmp[3] - '0') + (tmp[4] - '0');
        uint16_t g = 100 * (tmp[6] - '0') + 10 * (tmp[7] - '0') + (tmp[8] - '0');
        uint16_t b = 100 * (tmp[10] - '0') + 10 * (tmp[11] - '0') + (tmp[12] - '0');
        Serial.print("CMD: CHANGE COLOUR TO ");
        Serial.print(r);
        Serial.print(", ");
        Serial.print(g);
        Serial.print(", ");
        Serial.println(b);
        RGB_Set(r, g, b);
      } else if (tmp.startsWith("m")) {
        uint16_t r = 100 * (tmp[2] - '0') + 10 * (tmp[3] - '0') + (tmp[4] - '0');
        uint16_t g = 100 * (tmp[6] - '0') + 10 * (tmp[7] - '0') + (tmp[8] - '0');
        uint16_t b = 100 * (tmp[10] - '0') + 10 * (tmp[11] - '0') + (tmp[12] - '0');
        Serial.print("CMD: CHANGE COLOUR TO ");
        Serial.print(r);
        Serial.print(", ");
        Serial.print(g);
        Serial.print(", ");
        Serial.println(b);
        MAIN_Set(r, g, b);
      } else {
        Serial.println("UNKNOWN COMMAND!"); 
      }
    }
  }
}

void RGB_Set(const int r, const int g, const int b) {
  ledcWrite(0, r);
  ledcWrite(1, g);
  ledcWrite(2, b);
}

void MAIN_Set(const int r, const int g, const int b) {
  ledcWrite(3, r);
  ledcWrite(4, g);
  ledcWrite(5, b);
}

void ConnectToWiFi() {
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to "); Serial.println(ssid);
 
  uint16_t i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
 
    if ((++i % 200) == 0)
    {
      Serial.println(F(" still trying to connect"));
    }
  }
 
  Serial.print(F("Connected. My IP address is: "));
  Serial.println(WiFi.localIP());
  udp.begin(57222);
}

void all_off() {
  for (int i = 0; i < num_PWM; i++) {
        lights[i].turn_off();
  }
  for (int i = 0; i < 10 - num_PWM; i++) {
    no_PWM_lights[i].turn_off();
  }
  lamp.turn_off();
  speak.turn_off();
}

void all_test() {
  for (int i = 0; i < num_PWM; i++) {
        lights[i].test_me();
  }
  for (int i = 0; i < 10 - num_PWM; i++) {
    no_PWM_lights[i].test_me();
  }
  lamp.test_me();
  speak.test_me();
}

bool checkDead() {
  uint8_t buffer[50] = "";
  memset(buffer, 0, 50);
  udp.parsePacket();
  String tmp;
  if(udp.read(buffer, 50) > 0){
    tmp = String((char *) buffer);
    Serial.println(tmp);
    if (tmp.startsWith("Test")) {
      Serial.println("CMD: TEST");
      all_off(); 
      all_test();
      all_off();
      delay(200);
    } else if (tmp.startsWith("Silence")) {
      Serial.println("CMD: SILENCE");
      SILENCE = true;
    } else if (tmp.startsWith("Sound")) {
      Serial.println("CMD: SOUND");
      speak.test_me();
      SILENCE = false;
    } else if (tmp.startsWith("Stop!")) {
      Serial.println("CMD: STOP");
      all_off();
      STOP = true;
    } else if (tmp.startsWith("Start!")) {
      STOP = false;
      Serial.println("CMD: START");
    } else if (tmp.startsWith("Manual")) {
      Serial.println("CMD: MANUAL");
      all_off();
      manual_override();
    } else {
      Serial.println("UNKNOWN COMMAND!");
    }
  }
}
