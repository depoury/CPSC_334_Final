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
uint8_t RGB_Colours[] = {0, 255, 0};
uint8_t MAIN_Colours[] = {255, 0, 0};
uint8_t RGB_dec = 1;
uint8_t MAIN_dec = 0;
uint8_t val = 0;

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
}

void loop() {
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

  if (speak.switched_on) {
    speak.run_me();
  } else if (random(0, 1001) < prob_SPEAK) {
    speak.start_me();
  }
  delay(10);
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
