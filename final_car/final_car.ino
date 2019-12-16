#include <VirtualWire.h>

// front l9110s
#define F_BIA 9 // pwm
#define F_BIB 8
#define F_AIA 6 // pwm
#define F_AIB 7 
// back l9110s
#define B_AIA 3 // pwm
#define B_AIB 2
#define B_BIA 5 // pwm
#define B_BIB 4

#define FL_PWM F_BIA
#define FL_DIR F_BIB
#define FR_PWM F_AIA
#define FR_DIR F_AIB
#define BL_PWM B_AIA
#define BL_DIR B_AIB
#define BR_PWM B_BIA
#define BR_DIR B_BIB

#define RX_PIN 11

void setup() {
  Serial.begin(115200); // open the serial port at 115200 bps:
  pinMode(FL_PWM, OUTPUT);
  pinMode(FL_DIR, OUTPUT);
  pinMode(FR_PWM, OUTPUT);
  pinMode(FR_DIR, OUTPUT);
  pinMode(BL_PWM, OUTPUT);
  pinMode(BL_DIR, OUTPUT);
  pinMode(BR_PWM, OUTPUT);
  pinMode(BR_DIR, OUTPUT);

  // configure rf receiver
  vw_set_rx_pin(RX_PIN); // pin
  vw_setup(2000); // bps
  vw_rx_start();
}

//void forward(int speed, int duration) {
//  int dir = speed >= 0;
//  int speed_l = abs(speed);
//  int speed_r = speed_l;
//  if (dir  == 1) {
//    speed_r = 255 - speed_r;
//  } else {
//    speed_l = 255 - speed_l;
//  }
//
//  Serial.println("dir: " + String(dir) + " - speed_r: " + String(speed_r) + " - speed_l: " + String(speed_l));
//  digitalWrite(FR_IA, dir);
//  analogWrite(FR_IB, speed_r);
//  digitalWrite(FL_IA, !dir);
//  analogWrite(FL_IB, speed_l);
//  digitalWrite(BR_IA, dir);
//  analogWrite(BR_IB, speed_r);
//  digitalWrite(BL_IA, !dir);
//  analogWrite(BL_IB, speed_l);
//  delay(duration);
//}
//
//void sideways(int speed, int duration) {
//  int dir = speed >= 0;
//  int speed_l = abs(speed);
//  int speed_r = speed_l;
//  if (dir  == 1) {
//    speed_r = 255 - speed_r;
//  } else {
//    speed_l = 255 - speed_l;
//  }
//
//  Serial.println("dir: " + String(dir) + " - speed_r: " + String(speed_r) + " - speed_l: " + String(speed_l));
//  digitalWrite(FR_IA, !dir);
//  analogWrite(FR_IB, speed_l);
//  digitalWrite(FL_IA, !dir);
//  analogWrite(FL_IB, speed_l);
//  digitalWrite(BR_IA, dir);
//  analogWrite(BR_IB, speed_r);
//  digitalWrite(BL_IA, dir);
//  analogWrite(BL_IB, speed_r);
//  delay(duration);
//}

void move_vec(double x, double y, int speed) {
  double x_sq = x > 0 ? sq(x) : -sq(x);
  double y_sq = y > 0 ? sq(y) : -sq(y);
  double m = sqrt(abs(x_sq) + abs(y_sq));
//  Serial.println("x: " + String(x) + " - y: " + String(y));
//  int speed_1 = sqrt(y_sq + x_sq) / m * speed;
//  int speed_2 = sqrt(y_sq - x_sq) / m * speed;
  int speed_1 = y_sq + x_sq;
  int speed_2 = y_sq - x_sq;
  speed_1 = speed_1 > 0 ? sqrt(speed_1) / m * speed : -sqrt(abs(speed_1)) / m * speed;
  speed_2 = speed_2 > 0 ? sqrt(speed_2) / m * speed : -sqrt(abs(speed_2)) / m * speed;
//  Serial.println("speed_1: " + String(speed_1) + " - speed_2: " + String(speed_2));

  set_motors(speed_1, speed_2, speed_2, speed_1);
}

void set_motors(int speed_fl, int speed_fr, int speed_bl, int speed_br) {
  Serial.println("Set motors -- fl: " + String(speed_fl) + " - fr: " + String(speed_fr) + " - bl: " + String(speed_bl) + " - br: " + String(speed_br));
  digitalWrite(FL_DIR, speed_fl >= 0 ? HIGH : LOW);
  analogWrite(FL_PWM, speed_fl >= 0 ? 255 - speed_fl : abs(speed_fl));
  digitalWrite(FR_DIR, speed_fr <= 0 ? HIGH : LOW);
  analogWrite(FR_PWM, speed_fr <= 0 ? 255 - abs(speed_fr) : speed_fr);
  digitalWrite(BL_DIR, speed_bl >= 0 ? HIGH : LOW);
  analogWrite(BL_PWM, speed_bl >= 0 ? 255 - speed_bl : abs(speed_bl));
  digitalWrite(BR_DIR, speed_br <= 0 ? HIGH : LOW);
  analogWrite(BR_PWM, speed_br <= 0 ? 255 - abs(speed_br) : speed_br);
}

void get_imu_data() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) { // Non-blocking
    int i;
    digitalWrite(13, true); // Flash a light to show received good message
    // Message with a good checksum received, dump it.
    Serial.print("Got: ");

    int ax = atoi(buf);
    Serial.print(ax);
//    for (i = 0; i < buflen; i++) {
//       
//      Serial.print(buf[i], HEX);
//      Serial.print(" ");
//    }
    Serial.println("");
    digitalWrite(13, false);
  }
}

void loop() {
  get_imu_data();
//  delay(200);
//  set_motors(-255, 0, 0, 0);
//  delay(1500);

//  motor_test();
//  move_vec(0, 5, 255);  
//  delay(1500);
//  move_vec(5, 5, 255);
//  delay(1500);
//  move_vec(5, 0, 255);
//  delay(1500);
//  move_vec(5, -5, 255);
//  delay(1500);
//  move_vec(0, -5, 255);
//  delay(1500);
//  move_vec(-5, -5, 255);
//  delay(1500);
//  move_vec(-5, 0, 255);
//  delay(1500);
//  move_vec(-5, 5, 255);
//  delay(1500);
}

void motor_test() {
  set_motors(255, 0, 0, 0);
  delay(1500);
  set_motors(0, 255, 0, 0);
  delay(1500);
  set_motors(0, 0, 255, 0);
  delay(1500);
  set_motors(0, 0, 0, 255);
  delay(1500);
  set_motors(-255, 0, 0, 0);
  delay(1500);
  set_motors(0, -255, 0, 0);
  delay(1500);
  set_motors(0, 0, -255, 0);
  delay(1500);
  set_motors(0, 0, 0, -255);
  delay(1500);
}
