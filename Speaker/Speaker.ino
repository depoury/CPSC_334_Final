
const uint8_t s_pin = 9;
int freq = 0;

void setup() {
  Serial.begin(115200);
  ledcAttachPin(s_pin, 0);
  ledcSetup(0, 12000, 8);
}

void loop() {
  ledcWriteTone(0, freq++);
  freq = freq % 4096;
  delay(20);
}
