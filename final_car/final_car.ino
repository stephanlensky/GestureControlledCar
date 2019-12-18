#include <VirtualWire.h>

// front l9110s
#define F_BIA 11 // pwm (9 - > 11)
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

#define RX_PIN 12 // (11 -> 12)

#define NO_RX_PANIC_DELAY 2000
#define MIN_SPEED 200
#define MAX_SPEED 255
#define DEADZONE 25
#define MAX_PITCH 50
#define MAX_ROLL 50

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

void move_vec(double x, double y, int speed) {
  double x_sq = x > 0 ? sq(x) : -sq(x);
  double y_sq = y > 0 ? sq(y) : -sq(y);
  double m = sqrt(abs(x_sq) + abs(y_sq));
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
  delay(5);
}

int pitch, roll;
unsigned long last_data = 0;
void get_imu_data(boolean wait_rx) {
  uint8_t buf[VW_MAX_MESSAGE_LEN + 1];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (wait_rx) {
    Serial.println("Waiting for message...");
    vw_wait_rx();
  }
  
  if (vw_get_message(buf, &buflen)) { // Non-blocking
    digitalWrite(13, true); // Flash a light to show received good message
    buf[buflen] = '\0';
    int pitch_local, roll_local;
    sscanf(buf, "%d.%d", &pitch_local, &roll_local);
    
    // is data valid?
    if (pitch_local > 180 || pitch_local < -180 || roll_local > 180 || roll_local < -180) {
      Serial.print("IMU data is invalid! Received: "); Serial.println((char*) buf);
    } else {
      pitch = pitch_local;
      roll = roll_local;
      last_data = millis();
    }
    
    digitalWrite(13, false);
  }
}

boolean wait = true;
void loop() {
  get_imu_data(wait);
  wait = false;

  if (millis() > last_data + NO_RX_PANIC_DELAY) {
    Serial.println("Haven't received data in a while, stopping!");
    set_motors(0, 0, 0, 0);
    wait = true;
    return;
  }

  Serial.print(pitch); Serial.print("\t");
  Serial.println(roll);

  int y_dir = pitch > DEADZONE;
  if (pitch < -DEADZONE) {
    y_dir = -1;
  }

  int x_dir = roll > DEADZONE;
  if (roll < -DEADZONE) {
    x_dir = -1;
  }

  int speed;
  if (abs(pitch) > abs(roll)) {
    speed = (MAX_SPEED - MIN_SPEED) / (MAX_PITCH - DEADZONE) * (abs(pitch) - DEADZONE);
  } else {
    speed = (MAX_SPEED - MIN_SPEED) / (MAX_ROLL - DEADZONE) * (abs(roll) - DEADZONE);
  }
  move_vec(x_dir, y_dir, min(MIN_SPEED + speed, MAX_SPEED));
}

void motor_test() {
  set_motors(MAX_SPEED, 0, 0, 0);
  delay(1500);
  set_motors(0, MAX_SPEED, 0, 0);
  delay(1500);
  set_motors(0, 0, MAX_SPEED, 0);
  delay(1500);
  set_motors(0, 0, 0, MAX_SPEED);
  delay(1500);
  set_motors(-MAX_SPEED, 0, 0, 0);
  delay(1500);
  set_motors(0, -MAX_SPEED, 0, 0);
  delay(1500);
  set_motors(0, 0, -MAX_SPEED, 0);
  delay(1500);
  set_motors(0, 0, 0, -MAX_SPEED);
  delay(1500);
}
