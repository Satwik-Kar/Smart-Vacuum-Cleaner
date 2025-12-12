#define ENA 9
#define IN1 8
#define IN2 7
#define ENB 6
#define IN3 2
#define IN4 4

#define TRIG 11
#define ECHO 10
#define SERVO 12

#define IR_FRONT_LEFT A0
#define IR_FRONT_RIGHT A1

int motorSpeed = 255;
int baseLeft = 0;
int baseRight = 0;
const int tolerance = 150;
const int airThreshold = 800;

bool isMovingForward = false;

void setMotors(int pwm, int in1, int in2, int in3, int in4) {
  analogWrite(ENA, pwm);
  analogWrite(ENB, pwm);
  digitalWrite(IN1, in1); digitalWrite(IN2, in2);
  digitalWrite(IN3, in3); digitalWrite(IN4, in4);
}

void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  while (analogRead(IR_FRONT_LEFT) > airThreshold || analogRead(IR_FRONT_RIGHT) > airThreshold) {
    delay(100);
  }

  delay(1000);

  baseLeft = analogRead(IR_FRONT_LEFT);
  baseRight = analogRead(IR_FRONT_RIGHT);
}

void loop() {
  int currLeft = analogRead(IR_FRONT_LEFT);
  int currRight = analogRead(IR_FRONT_RIGHT);

  bool cliffDetected = (currLeft > (baseLeft + tolerance)) || (currRight > (baseRight + tolerance));

  if (cliffDetected && isMovingForward) {
    setMotors(0, LOW, LOW, LOW, LOW);
    isMovingForward = false;
  }

  if (Serial.available()) {
    char c = Serial.read();

    if (c >= '0' && c <= '9') {
      int val = c - '0';
      motorSpeed = map(val, 0, 9, 0, 255);
    }
    else {
      switch (c) {
        case 'F':
          if (!cliffDetected) {
            setMotors(motorSpeed, HIGH, LOW, HIGH, LOW);
            isMovingForward = true;
          } else {
            setMotors(0, LOW, LOW, LOW, LOW);
            isMovingForward = false;
          }
          break;

        case 'B':
          setMotors(motorSpeed, LOW, HIGH, LOW, HIGH);
          isMovingForward = false;
          break;

        case 'L':
          setMotors(motorSpeed, LOW, HIGH, HIGH, LOW);
          isMovingForward = false;
          break;

        case 'R':
          setMotors(motorSpeed, HIGH, LOW, LOW, HIGH);
          isMovingForward = false;
          break;

        case 'S':
          setMotors(0, LOW, LOW, LOW, LOW);
          isMovingForward = false;
          break;
      }
    }
  }
}