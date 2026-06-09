/**********************************************************
* @file         arduino_julieta.ino
* @board        Robocore Julieta v1.0
* @components   2 sg90 servos + 1 dc geared motor
* @author       FredericoAfra
**********************************************************/

#include <Servo.h>

// --- SERVO PINS ---
const int servoPin1 = 9;
const int servoPin2 = 10;

// --- TREADMILL MOTOR PINS ---
const int M1_Vel = 5;
const int M1_Dir = 7; // Adjusted from 6 to 7 (Native direction pin for Motor 1 on Julieta)

// --- INTEGRATED BUTTONS PINS ---
const int pinBtnOnOff = A0; // Integrated button "Btn0"
const int pinBtnDir   = A1; // Integrated button "Btn1"

Servo servo1;
Servo servo2;

// --- PRE-DEFINED POSITIONS (SERVO 1) ---
const int degPos1 = 10;
const int degPos2 = 90;
const int degPos3 = 170;

// --- MOTION CONFIG FOR SERVO 2 ---
const int degMinServo2 = 30; 
const int degMaxServo2 = 150; 
int currentDegServo2 = degMinServo2; 
int directionServo2 = 1; 
unsigned long lastTimeServo2 = 0;
const int stepIntervalServo2 = 15; // Time in milliseconds for each step

// --- CONTROL VARIABLES FOR TREADMILL ---
bool motorEnabled = false;       // Treadmill state: false = off, true = on
bool motorDirection = false;     // Treadmill direction: false = clockwise, true = counter-clockwise
bool lastBtnOnOffState = HIGH;   // Stores the last read state of the On/Off button
bool lastBtnDirState = HIGH;     // Stores the last read state of the Direction button

void setup() {
  // Initialize serial communication with the ESP32
  Serial.begin(9600);

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);

  // Initialize servos at their initial positions
  servo1.write(degPos1);
  servo2.write(currentDegServo2);

  // Configure motor output pins
  pinMode(M1_Vel, OUTPUT);
  pinMode(M1_Dir, OUTPUT);

  // Configure integrated buttons with Julieta's internal Pull-Up resistor
  pinMode(pinBtnOnOff, INPUT_PULLUP);
  pinMode(pinBtnDir, INPUT_PULLUP);
}

void loop() {
  // --- TASK 1: READ ESP32 MESSAGE (SERVO 1) ---
  if (Serial.available() > 0) {
    char command = Serial.read();

    switch (command) {
      case '1':
        servo1.write(degPos1);
        break;
      case '2':
        servo1.write(degPos2);
        break;
      case '3':
        servo1.write(degPos3);
        break;
      default:
        break;
    }
  }

  // --- TASK 2: CONSTANT MOTION (SERVO 2) ---
  if (millis() - lastTimeServo2 >= stepIntervalServo2) {
    lastTimeServo2 = millis();

    currentDegServo2 += directionServo2;
    servo2.write(currentDegServo2);

    if (currentDegServo2 <= degMinServo2 || currentDegServo2 >= degMaxServo2) {
      directionServo2 = -directionServo2;
    }
  }

  // --- TASK 3: TREADMILL CONTROL WITH INTEGRATED BUTTONS ---
  
  // 1. Monitor On/Off Button (Btn0 - A0)
  bool currentBtnOnOff = digitalRead(pinBtnOnOff);
  if (currentBtnOnOff == LOW && lastBtnOnOffState == HIGH) { 
    motorEnabled = !motorEnabled; // Toggles the state (turns on if off / turns off if on)
    delay(20);                    // Small 20ms debounce to prevent mechanical click noise
  }
  lastBtnOnOffState = currentBtnOnOff;

  // 2. Monitor Direction Button (Btn1 - A1)
  bool currentBtnDir = digitalRead(pinBtnDir);
  if (currentBtnDir == LOW && lastBtnDirState == HIGH) { 
    motorDirection = !motorDirection; // Toggles the treadmill rotation direction
    delay(20);                        // Small debounce
  }
  lastBtnDirState = currentBtnDir;

  // 3. Apply physical states to the treadmill motor
  if (motorEnabled) {
    analogWrite(M1_Vel, 255); // Full speed for the treadmill
    digitalWrite(M1_Dir, motorDirection ? HIGH : LOW); // Applies the current direction
  } else {
    analogWrite(M1_Vel, 0);   // Stops the treadmill by cutting the speed signal
  }
}
