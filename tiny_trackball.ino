// Tiny Trackball
// 2020-09-09  T. Nakagawa

#include <Mouse.h>

constexpr int INTERVAL = 10;  // 10ms HID polling interval.
constexpr int DURATION = 1000;  // Keep LED color for 1s.
constexpr int SCALE = 10;  // Sensivity of the trackball.
constexpr int PORT_RED = 9;
constexpr int PORT_BLU = 8;
constexpr int PORT_GRN = 6;
constexpr int PORT_WHT = 4;
constexpr int PORT_UP  = 1;
constexpr int PORT_DWN = 0;
constexpr int PORT_LFT = 3;
constexpr int PORT_RHT = 2;
constexpr int PORT_BTN = 7;
constexpr int PORT_TXL = 30;
constexpr int PORT_RXL = 17;

static volatile int cnt_up  = 0;
static volatile int cnt_dwn = 0;
static volatile int cnt_lft = 0;
static volatile int cnt_rht = 0;

void setup() {
  pinMode(PORT_BLU, OUTPUT);
  pinMode(PORT_RED, OUTPUT);
  pinMode(PORT_GRN, OUTPUT);
  pinMode(PORT_WHT, OUTPUT);
  pinMode(PORT_UP , INPUT);
  pinMode(PORT_DWN, INPUT);
  pinMode(PORT_LFT, INPUT);
  pinMode(PORT_RHT, INPUT);
  pinMode(PORT_BTN, INPUT);
  pinMode(PORT_TXL, INPUT);
  pinMode(PORT_RXL, INPUT);
  digitalWrite(PORT_GRN, HIGH);
  attachInterrupt(digitalPinToInterrupt(PORT_UP ), []() {cnt_up ++;}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PORT_DWN), []() {cnt_dwn++;}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PORT_LFT), []() {cnt_lft++;}, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PORT_RHT), []() {cnt_rht++;}, CHANGE);
  Mouse.begin();
}

void loop() {
  delay(INTERVAL);
  noInterrupts();
  const int dx = cnt_rht - cnt_lft;
  const int dy = cnt_dwn - cnt_up ;
  cnt_up  = 0;
  cnt_dwn = 0;
  cnt_lft = 0;
  cnt_rht = 0;
  interrupts();
  if (dx != 0 || dy != 0) Mouse.move(dx * SCALE, dy * SCALE);

  static int old_btn = HIGH;
  const bool new_btn = digitalRead(PORT_BTN);
  if (old_btn == HIGH && new_btn == LOW) Mouse.press();
  else if (old_btn == LOW && new_btn == HIGH) Mouse.release();
  old_btn = new_btn;

  static unsigned long timer = 0;
  const unsigned long now = millis();
  if (new_btn == LOW) {
    led(PORT_RED);
    timer = now + DURATION;
  } else if (dx != 0 || dy != 0) {
    led(PORT_WHT);
    timer = now + DURATION;
  } else if (timer != 0 && timer < now){
    led(PORT_GRN);
    timer = 0;
  }
}

void led(int port) {
  static int old_port = PORT_GRN;
  if (port == old_port) return;
  digitalWrite(old_port, LOW);
  digitalWrite(port, HIGH);
  old_port = port;
}
