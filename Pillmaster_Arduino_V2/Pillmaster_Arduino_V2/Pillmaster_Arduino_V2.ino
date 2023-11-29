#include <AccelStepper.h>
#include <MultiStepper.h>
/*
PillMaster Stepper Program
  running through A4988 driver board
  NEMA 17 17he08 stepper motors
  Full rotation is 200 steps
*/
#define STEPS 200
#define MANUAL_STEPS 5
#define TO_DROP_STEPS 50
#define MEDIUM_OPENING_STEPS 20
#define DEFAULT_SPEED 100

//flags are organized by 0x<commandflag><canisterflag>
//0x58 (ascii 'X') is operate (0x5) canister d (0x8)
const unsigned char mainFlag = 0x1;
const unsigned char revFlag = 0x4;
const unsigned char adjFlag = 0x3;
const unsigned char bothFlag = 0x2;
const unsigned char operateFlag = 0x5;
const unsigned char eflag = 0x9;
const unsigned char dflag = 0x8;
const unsigned char cflag = 0x7;
const unsigned char bflag = 0x6;
const unsigned char aflag = 0x5;

const int max_operations = 6;
const int operation_medium_main[] = {  0,-50,   0, 0,  0, 50};
const int operation_medium_adjs[] = {-20, 50, -30, 0, 50,-50};
const int operation_small_main[] = {  0,-50,   0, 0,  0, 50};
const int operation_small_adjs[] = {-20, 50, -30, 0, 50,-50};

const int dirPinMain = 2;
const int stepPinMain = 3;
const int dirPinAdj = 5;
const int stepPinAdj = 6;

const int dirPinMain2 = 9;
const int stepPinMain2 = 10;
const int dirPinAdj2 = 11;
const int stepPinAdj2 = 12;

const int enPinA = 4;
const int enPinB = 7;
const int enPinC = 8;
const int enPinD = A0;
const int enPinE = A1;

unsigned char currentMotor = aflag;

long positionA = 0;
long positionB = 0;
long positionC = 0;
long positionD = 0;
long positionE = 0;

#define motorInterfaceType 1

AccelStepper mnStepper(AccelStepper::DRIVER, stepPinMain, dirPinMain);
AccelStepper ajStepper(AccelStepper::DRIVER, stepPinAdj, dirPinAdj);

AccelStepper mnStepper2(AccelStepper::DRIVER, stepPinMain2, dirPinMain2);
AccelStepper ajStepper2(AccelStepper::DRIVER, stepPinAdj2, dirPinAdj2);

void setup() {
  Serial.begin(9600);
  //enable pins
  pinMode(A5, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A0, OUTPUT);

  mnStepper.setMaxSpeed(DEFAULT_SPEED);
  ajStepper.setMaxSpeed(DEFAULT_SPEED);

  mnStepper2.setMaxSpeed(DEFAULT_SPEED);
  ajStepper2.setMaxSpeed(DEFAULT_SPEED);


  swapMotor(aflag);
}

/*
switch which motor is enabled
enable pins are active LOW
*/
void swapMotor(unsigned char val) {
  if (currentMotor != val) {
    currentMotor = val;
  }
  switch (val) {
    case aflag:
      digitalWrite(enPinA, LOW);
      digitalWrite(enPinB, HIGH);
      digitalWrite(enPinC, HIGH);
      digitalWrite(enPinD, HIGH);
      digitalWrite(enPinE, HIGH);
      break;
    case bflag:
      digitalWrite(enPinA, HIGH);
      digitalWrite(enPinB, LOW);
      digitalWrite(enPinC, HIGH);
      digitalWrite(enPinD, HIGH);
      digitalWrite(enPinE, HIGH);
      break;
    case cflag:
      digitalWrite(enPinA, HIGH);
      digitalWrite(enPinB, HIGH);
      digitalWrite(enPinC, LOW);
      digitalWrite(enPinD, HIGH);
      digitalWrite(enPinE, HIGH);
      break;
    case dflag:
      digitalWrite(enPinA, HIGH);
      digitalWrite(enPinB, HIGH);
      digitalWrite(enPinC, HIGH);
      digitalWrite(enPinD, LOW);
      digitalWrite(enPinE, HIGH);
      break;
    case eflag:
      digitalWrite(enPinA, HIGH);
      digitalWrite(enPinB, HIGH);
      digitalWrite(enPinC, HIGH);
      digitalWrite(enPinD, HIGH);
      digitalWrite(enPinE, LOW);
      break;
  }
  delay(100);
}

void resetSpeed() {
  mnStepper.setSpeed(DEFAULT_SPEED);
  ajStepper.setSpeed(DEFAULT_SPEED);
  mnStepper2.setSpeed(DEFAULT_SPEED);
  ajStepper2.setSpeed(DEFAULT_SPEED);
}

void updateTarget(unsigned char flag, unsigned char canister) {
  if (canister == eflag || canister == dflag) {
    if (flag == operateFlag) {
      readInstructions2();
    } else if (flag == mainFlag) {
      mnStepper2.move(MANUAL_STEPS);
      ajStepper2.move(-MANUAL_STEPS);
      resetSpeed();
    } else if (flag == revFlag) {
      Serial.print("here");
      mnStepper2.move(-MANUAL_STEPS);
      ajStepper2.move(MANUAL_STEPS);
      resetSpeed();
    } else if (flag == adjFlag) {
      ajStepper2.move(-MANUAL_STEPS);
      resetSpeed();
    } else if (flag == bothFlag) {
      ajStepper2.move(MANUAL_STEPS);
      resetSpeed();
    }
  } else if (canister == aflag || canister == bflag || canister == cflag) {
    if (flag == operateFlag) {
      readInstructions();
    } else if (flag == mainFlag) {
      mnStepper.move(MANUAL_STEPS);
      ajStepper.move(-MANUAL_STEPS);
      resetSpeed();
    } else if (flag == revFlag) {
      mnStepper.move(-MANUAL_STEPS);
      ajStepper.move(MANUAL_STEPS);
      resetSpeed();
    } else if (flag == adjFlag) {
      ajStepper.move(-MANUAL_STEPS);
      resetSpeed();
    } else if (flag == bothFlag) {
      ajStepper.move(MANUAL_STEPS);
      resetSpeed();
    }
  }
}
void readInstructions() {
  for(int i = 0; i < max_operations; i++) {
    int main = operation_medium_main[i];
    int adjs = operation_medium_adjs[i];
    
    if(main == 0 && adjs == 0) {
      //do nothing
    } else {
      mnStepper.move(main);
      ajStepper.move(adjs);
      resetSpeed();
      delay(250);
      while (mnStepper.distanceToGo() != 0 || ajStepper.distanceToGo() != 0) {
        mnStepper.runSpeedToPosition();
        ajStepper.runSpeedToPosition();
      }
    }
  }
  Serial.println("1");
}

void readInstructions2() {
  for(int i = 0; i < max_operations; i++) {
    int main = operation_medium_main[i];
    int adjs = operation_medium_adjs[i];
    
    if(main == 0 && adjs == 0) {
      //do nothing
    } else {
      mnStepper2.move(main);
      ajStepper2.move(adjs);
      resetSpeed();
      delay(250);
      while (mnStepper2.distanceToGo() != 0 || ajStepper2.distanceToGo() != 0) {
        mnStepper2.runSpeedToPosition();
        ajStepper2.runSpeedToPosition();
      }
    }
  }
  Serial.println("1");
}

void loop() {
  long positions[2];
  if (Serial.available() > 0) {
    byte serialByte = Serial.read();
    unsigned char canister = serialByte & 0xf;
    unsigned char flags = (unsigned char)(serialByte >> 4);
    swapMotor(canister);
    updateTarget(flags, canister);
  }
  mnStepper.runSpeedToPosition();
  ajStepper.runSpeedToPosition();
  mnStepper2.runSpeedToPosition();
  ajStepper2.runSpeedToPosition();
}