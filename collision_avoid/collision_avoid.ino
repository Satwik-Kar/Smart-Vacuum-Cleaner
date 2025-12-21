#include <Servo.h>

#define ENA 9
#define IN1 8
#define IN2 7
#define ENB 6
#define IN3 2
#define IN4 4

#define TRIG 11
#define ECHO 10
#define SERVO_PIN 12

#define IR_FRONT_LEFT A0
#define IR_FRONT_RIGHT A1

Servo headServo;

const int motorSpeed = 255; 
bool isAutoMode = true;

int baseLeft = 0;
int baseRight = 0;
const int tolerance = 150;
const int obstacleDistCm = 25;

const int TIME_TURN_90 = 500;
const int TIME_LANE_SHIFT = 600;

void setMotors(int pwm, int in1, int in2, int in3, int in4) {
  analogWrite(ENA, pwm);
  analogWrite(ENB, pwm);
  digitalWrite(IN1, in1); digitalWrite(IN2, in2);
  digitalWrite(IN3, in3); digitalWrite(IN4, in4);
}

void stopBot() {
  setMotors(0, LOW, LOW, LOW, LOW);
}

int getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH, 25000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(9600);
  headServo.attach(SERVO_PIN);
  headServo.write(90);

  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);

  delay(1000);
  baseLeft = analogRead(IR_FRONT_LEFT);
  baseRight = analogRead(IR_FRONT_RIGHT);
}

void performLaneSwitch(bool turnRight) {
  stopBot();
  delay(200);

  if (turnRight) {
    setMotors(motorSpeed, HIGH, LOW, LOW, HIGH);
  } else {
    setMotors(motorSpeed, LOW, HIGH, HIGH, LOW);
  }
  delay(TIME_TURN_90);
  stopBot();
  delay(200);
  
  setMotors(motorSpeed, HIGH, LOW, HIGH, LOW);
  delay(TIME_LANE_SHIFT);
  stopBot();
  delay(200);
  
  if (turnRight) {
    setMotors(motorSpeed, HIGH, LOW, LOW, HIGH);
  } else {
    setMotors(motorSpeed, LOW, HIGH, HIGH, LOW);
  }
  delay(TIME_TURN_90);
  stopBot();
  delay(200);
}

void handleObstacle() {
  stopBot();
  
  headServo.write(0); 
  delay(400);
  int rightDist = getDistance();
  
  headServo.write(180); 
  delay(400);
  int leftDist = getDistance();
  
  headServo.write(90); 
  delay(200);

  if (rightDist > leftDist) {
    performLaneSwitch(true);
  } else {
    performLaneSwitch(false);
  }
}

void runAutoLogic() {
  int currLeft = analogRead(IR_FRONT_LEFT);
  int currRight = analogRead(IR_FRONT_RIGHT);
  int dist = getDistance();

  bool cliffDetected = (currLeft > (baseLeft + tolerance)) || (currRight > (baseRight + tolerance));

  if (cliffDetected) {
    stopBot();
    setMotors(motorSpeed, LOW, HIGH, LOW, HIGH); 
    delay(400);
    setMotors(motorSpeed, HIGH, LOW, LOW, HIGH); 
    delay(500);
    stopBot();
  } 
  else if (dist < obstacleDistCm) {
    handleObstacle();
  }
  else {
    setMotors(motorSpeed, HIGH, LOW, HIGH, LOW);
  }
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    
    if (c == 'A') {
      isAutoMode = true;
    } else if (c == 'M') {
      isAutoMode = false;
      stopBot();
    } else if (!isAutoMode) {
      switch (c) {
        case 'F': setMotors(motorSpeed, HIGH, LOW, HIGH, LOW); break;
        case 'B': setMotors(motorSpeed, LOW, HIGH, LOW, HIGH); break;
        case 'L': setMotors(motorSpeed, LOW, HIGH, HIGH, LOW); break;
        case 'R': setMotors(motorSpeed, HIGH, LOW, LOW, HIGH); break;
        case 'S': stopBot(); break;
      }
    }
  }

  if (isAutoMode) {
    runAutoLogic();
  }
}