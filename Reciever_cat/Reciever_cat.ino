#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Adafruit_PWMServoDriver.h>

typedef struct struct_message {
  int16_t x;            // -1000 to +1000
  int16_t y;            // -1000 to +1000
  bool buttonPressed;   // Switch/button state
} struct_message;

struct_message receivedData;

//PCA9685 setup
#define PCA9685_ADDRESS 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PCA9685_ADDRESS);

// Servo channels according to the PCA9685
#define SERVO_1_CH 0  // Front Right
#define SERVO_2_CH 1  // Front Left
#define SERVO_3_CH 2  // Rack and Pinion
#define SERVO_4_CH 3  // Back Right
#define SERVO_5_CH 4  // Back Left
#define SERVO_6_CH 5  // Left Rotator
#define SERVO_7_CH 6  // Right Rotator

// Servo pulse width constants
#define SERVO_MIN_PULSE 500   // µs for 0 degrees
#define SERVO_MAX_PULSE 2400  // µs for 180 degrees

// Track current angles
int currentAngles[7] = {90, 90, 90, 90, 100, 90, 90};

// Flags
volatile bool turnLeftFlag = false;
volatile bool turnRightFlag = false;
volatile bool walkForwardFlag = false;
volatile bool walkBackwardFlag = false;
volatile bool buttonPressedFlag = false;  // Button state from transmitter

bool isWalking = false;
bool isTurning = false;
bool leftTurnInitialized = false;
bool rightTurnInitialized = false;
bool isButtonActionRunning = false;  // Track if button action is currently running

// Set servo angle
void setServoAngle(uint8_t channel, int angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  uint16_t pulse = map(angle, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  pwm.writeMicroseconds(channel, pulse);
}

// GET / UPDATE CURRENT ANGLE
int getCurrentAngle(uint8_t channel) {
  return currentAngles[channel];
}

void updateCurrentAngle(uint8_t channel, int angle) {
  currentAngles[channel] = angle;
}

// ESP-NOW CALLBACK: DATA RECEIVED
void onReceive(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
  memcpy(&receivedData, data, sizeof(receivedData));
  
  int x = receivedData.x;
  int y = receivedData.y;
  bool button = receivedData.buttonPressed;
  
  // ----- BUTTON STATE -----
  buttonPressedFlag = button;
  
  // ----- X-AXIS: TURN LEFT (x < -500) / TURN RIGHT (x > 500) -----
  if (x < -500) {
    turnLeftFlag = true;
    turnRightFlag = false;
  } else if (x > 500) {
    turnRightFlag = true;
    turnLeftFlag = false;
  } else {
    turnLeftFlag = false;
    turnRightFlag = false;
  }
  
  // ----- Y-AXIS: WALK FORWARD (y > 500) / WALK BACKWARD (y < -500) -----
  if (y > 500) {
    walkForwardFlag = true;
    walkBackwardFlag = false;
  } else if (y < -500) {
    walkBackwardFlag = true;
    walkForwardFlag = false;
  } else {
    walkForwardFlag = false;
    walkBackwardFlag = false;
  }
}

// BUTTON ACTION
void performButtonAction() {
  if (!buttonPressedFlag) {
    // Button was released - exit and reset to idle
    if (isButtonActionRunning) {
      isButtonActionRunning = false;
      resetToIdle();
    }
    return;
  }
  
  if (!isButtonActionRunning) {
    isButtonActionRunning = true;
    isWalking = false;
    isTurning = false;
    leftTurnInitialized = false;
    rightTurnInitialized = false;
    Serial.println("BUTTON HELD - Running reset sequence");
  }
  
  //BUTTON SEQUENCE
  setServoAngle(SERVO_6_CH, 90);
  updateCurrentAngle(SERVO_6_CH, 90);
  setServoAngle(SERVO_7_CH, 82);
  updateCurrentAngle(SERVO_7_CH, 82);
  slowMove(SERVO_3_CH, 90, 150);
  delay(20); 

  fastMoveTwoServos(SERVO_2_CH, 140, SERVO_1_CH, 40, 300);
  delay(20); 
  
}

// RIGHT TURN SETUP 
void rightTurnSetup() {
  Serial.println("RIGHT TURN SETUP - Running once");
  
  setServoAngle(SERVO_1_CH, 90);
  updateCurrentAngle(SERVO_1_CH, 90);
  setServoAngle(SERVO_2_CH, 90);
  updateCurrentAngle(SERVO_2_CH, 90);
  setServoAngle(SERVO_4_CH, 90);
  updateCurrentAngle(SERVO_4_CH, 90);
  setServoAngle(SERVO_5_CH, 100);
  updateCurrentAngle(SERVO_5_CH, 100);
  
  fastMoveSingleServo(SERVO_3_CH, 65, 100);
  delay(20);
  fastMoveSingleServo(SERVO_6_CH, 40, 100);
  delay(20);
  fastMoveSingleServo(SERVO_7_CH, 32, 100);
  delay(20);
  
  rightTurnInitialized = true;
}

// RIGHT TURN LOOP
void rightTurnLoop() {
  // Lock idle servos
  setServoAngle(SERVO_1_CH, 90);
  updateCurrentAngle(SERVO_1_CH, 90);
  setServoAngle(SERVO_4_CH, 90);
  updateCurrentAngle(SERVO_4_CH, 90);
  setServoAngle(SERVO_5_CH, 100);
  updateCurrentAngle(SERVO_5_CH, 100);

  fastMoveSingleServo(SERVO_2_CH, 115, 100);  
  delay(20);
  
  fastMoveSingleServo(SERVO_3_CH, 125, 100);
  delay(20);
  
  fastMoveSingleServo(SERVO_2_CH, 55, 100); 
  delay(20);
  
  fastMoveSingleServo(SERVO_3_CH, 65, 100);
  delay(20);
}

// LEFT TURN SETUP
void leftTurnSetup() {
  Serial.println("LEFT TURN SETUP - Running once");
  
  setServoAngle(SERVO_1_CH, 90);
  updateCurrentAngle(SERVO_1_CH, 90);
  setServoAngle(SERVO_2_CH, 90);
  updateCurrentAngle(SERVO_2_CH, 90);
  setServoAngle(SERVO_4_CH, 90);
  updateCurrentAngle(SERVO_4_CH, 90);
  setServoAngle(SERVO_5_CH, 100);
  updateCurrentAngle(SERVO_5_CH, 100);
  
  fastMoveSingleServo(SERVO_3_CH, 125, 100);
  delay(20);
  fastMoveSingleServo(SERVO_7_CH, 132, 100);
  delay(20);
  fastMoveSingleServo(SERVO_6_CH, 140, 100);
  delay(20);
  
  leftTurnInitialized = true;
}

// LEFT TURN LOOP
void leftTurnLoop() {
  // Lock idle servos
  setServoAngle(SERVO_2_CH, 90);
  updateCurrentAngle(SERVO_2_CH, 90);
  setServoAngle(SERVO_4_CH, 90);
  updateCurrentAngle(SERVO_4_CH, 90);
  setServoAngle(SERVO_5_CH, 100);
  updateCurrentAngle(SERVO_5_CH, 100);

  fastMoveSingleServo(SERVO_1_CH, 115, 100); 
  delay(20);
  
  fastMoveSingleServo(SERVO_3_CH, 125, 100);
  delay(20);
  
  fastMoveSingleServo(SERVO_1_CH, 55, 100);  
  delay(20);
  
  fastMoveSingleServo(SERVO_3_CH, 65, 100);
  delay(20);
}

// WALK FORWARD
void walkForward() {
  if (!isWalking) {
    isWalking = true;
    isTurning = false;
    Serial.println("Walking FORWARD (continuous)");
  }
  
  leftTurnInitialized = false;
  rightTurnInitialized = false;
  
  setServoAngle(SERVO_6_CH, 90);
  updateCurrentAngle(SERVO_6_CH, 90);
  setServoAngle(SERVO_7_CH, 82);
  updateCurrentAngle(SERVO_7_CH, 82);
  
  // Complete one walking cycle
  slowMove(SERVO_3_CH, 125, 150); // R&P drops
  delay(20); 

  fastMoveTwoServos(SERVO_2_CH, 70, SERVO_4_CH, 110, 300);
  delay(20);

  fastMoveTwoServos(SERVO_1_CH, 70, SERVO_5_CH, 120, 300);
  delay(20);
  
  slowMove(SERVO_3_CH, 65, 150);
  delay(20);

  fastMoveTwoServos(SERVO_2_CH, 110, SERVO_4_CH, 70, 300);
  delay(20); 

  fastMoveTwoServos(SERVO_1_CH, 110, SERVO_5_CH, 80, 300);
  delay(20); 
}

// WALK BACKWARD
void walkBackward() {
  if (!isWalking) {
    isWalking = true;
    isTurning = false;
    Serial.println("Walking BACKWARD (continuous)");
  }
  
  leftTurnInitialized = false;
  rightTurnInitialized = false;
  
  setServoAngle(SERVO_6_CH, 90);
  updateCurrentAngle(SERVO_6_CH, 90);
  setServoAngle(SERVO_7_CH, 82);
  updateCurrentAngle(SERVO_7_CH, 82);
  
  fastMoveSingleServo(SERVO_3_CH, 75, 150);
  delay(20);
  fastMoveTwoServos(SERVO_2_CH, 70, SERVO_4_CH, 110, 300);
  delay(20);
  fastMoveTwoServos(SERVO_1_CH, 70, SERVO_5_CH, 120, 300);
  delay(20);
  fastMoveSingleServo(SERVO_3_CH, 115, 150);
  delay(20);
  fastMoveTwoServos(SERVO_2_CH, 110, SERVO_4_CH, 70, 300);
  delay(20);
  fastMoveTwoServos(SERVO_1_CH, 110, SERVO_5_CH, 80, 300);
  delay(20);
}

// RESET TO IDLE
void resetToIdle() {
  isWalking = false;
  isTurning = false;
  isButtonActionRunning = false;
  leftTurnInitialized = false;
  rightTurnInitialized = false;
  Serial.println("Stopped - returning to idle");
  
  fastMoveSingleServo(SERVO_1_CH, 90, 300);
  fastMoveSingleServo(SERVO_2_CH, 90, 300);
  fastMoveSingleServo(SERVO_3_CH, 93, 300);
  fastMoveSingleServo(SERVO_4_CH, 90, 300);
  fastMoveSingleServo(SERVO_5_CH, 100, 300);
  fastMoveSingleServo(SERVO_6_CH, 90, 300);
  fastMoveSingleServo(SERVO_7_CH, 82, 300);
}

// SERVO MOVEMENT FUNCTIONS
void slowMove(uint8_t channel, int targetAngle, int duration) {
  int currentAngle = getCurrentAngle(channel);
  int totalDegrees = abs(targetAngle - currentAngle);
  if (totalDegrees == 0) return;
  
  int delayPerDegree = duration / totalDegrees;
  if (delayPerDegree < 15) delayPerDegree = 15;
  
  if (currentAngle < targetAngle) {
    for (int angle = currentAngle; angle <= targetAngle; angle++) {
      setServoAngle(channel, angle);
      updateCurrentAngle(channel, angle);
      delay(delayPerDegree);
    }
  } else {
    for (int angle = currentAngle; angle >= targetAngle; angle--) {
      setServoAngle(channel, angle);
      updateCurrentAngle(channel, angle);
      delay(delayPerDegree);
    }
  }
  
  setServoAngle(channel, targetAngle);
  updateCurrentAngle(channel, targetAngle);
  delay(15);
}

void fastMoveSingleServo(uint8_t channel, int targetAngle, int duration) {
  int currentAngle = getCurrentAngle(channel);
  int totalDegrees = abs(targetAngle - currentAngle);
  if (totalDegrees == 0) return;
  
  int stepSize = 3;
  int steps = totalDegrees / stepSize;
  if (steps == 0) steps = 1;
  
  int delayPerStep = duration / steps;
  if (delayPerStep < 5) delayPerStep = 5;
  
  int direction = (currentAngle < targetAngle) ? 1 : -1;
  
  for (int step = 1; step <= steps; step++) {
    int newAngle = currentAngle + (direction * step * stepSize);
    if (step == steps) newAngle = targetAngle;
    setServoAngle(channel, newAngle);
    updateCurrentAngle(channel, newAngle);
    delay(delayPerStep);
  }
  
  setServoAngle(channel, targetAngle);
  updateCurrentAngle(channel, targetAngle);
  delay(10);
}

void fastMoveTwoServos(uint8_t channelA, int targetA, uint8_t channelB, int targetB, int duration) {
  int currentA = getCurrentAngle(channelA);
  int currentB = getCurrentAngle(channelB);
  
  int totalDegreesA = abs(targetA - currentA);
  int totalDegreesB = abs(targetB - currentB);
  
  int stepSize = 3;
  int stepsA = totalDegreesA / stepSize;
  int stepsB = totalDegreesB / stepSize;
  int maxSteps = max(stepsA, stepsB);
  if (maxSteps == 0) return;
  
  int delayPerStep = duration / maxSteps;
  if (delayPerStep < 5) delayPerStep = 5;
  
  int dirA = (currentA < targetA) ? 1 : -1;
  int dirB = (currentB < targetB) ? 1 : -1;
  
  for (int step = 0; step <= maxSteps; step++) {
    if (stepsA > 0) {
      int angleA = currentA + (dirA * step * totalDegreesA / maxSteps);
      setServoAngle(channelA, angleA);
      updateCurrentAngle(channelA, angleA);
    }
    if (stepsB > 0) {
      int angleB = currentB + (dirB * step * totalDegreesB / maxSteps);
      setServoAngle(channelB, angleB);
      updateCurrentAngle(channelB, angleB);
    }
    delay(delayPerStep);
  }
  
  setServoAngle(channelA, targetA);
  updateCurrentAngle(channelA, targetA);
  setServoAngle(channelB, targetB);
  updateCurrentAngle(channelB, targetB);
  delay(10);
}

// SETUP
void setup() {
  Serial.begin(115200);
  Serial.println("Receiver starting with PCA9685...");
  
  //INITIALIZE PCA9685
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);
  delay(10);
  
  //SET INITIAL POSITIONS
  setServoAngle(SERVO_1_CH, 90);
  currentAngles[SERVO_1_CH] = 90;
  delay(50);
  setServoAngle(SERVO_2_CH, 90);
  currentAngles[SERVO_2_CH] = 90;
  delay(50);
  setServoAngle(SERVO_3_CH, 93);
  currentAngles[SERVO_3_CH] = 93;
  delay(50);
  setServoAngle(SERVO_4_CH, 90);
  currentAngles[SERVO_4_CH] = 90;
  delay(50);
  setServoAngle(SERVO_5_CH, 100);
  currentAngles[SERVO_5_CH] = 100;
  delay(50);
  setServoAngle(SERVO_6_CH, 90);
  currentAngles[SERVO_6_CH] = 90;
  delay(50);
  setServoAngle(SERVO_7_CH, 82);
  currentAngles[SERVO_7_CH] = 82; //Ideally, have all of these angles at 90 (in idle position too)
  //If you have to offset some angles, offset them for every movement!
  
  delay(5000);
  
  //ESP-NOW INIT
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (1);
  }
  
  esp_now_register_recv_cb(onReceive);
  Serial.println("Receiver ready!");
}

// MAIN LOOP
void loop() {
  // ----- TURN LEFT (In order of priority) -----
  if (turnLeftFlag) {
    isTurning = true;
    isWalking = false;
    isButtonActionRunning = false; 
    
    if (!leftTurnInitialized) {
      leftTurnSetup();
    }
    
    leftTurnLoop();
  } 
  // ----- TURN RIGHT -----
  else if (turnRightFlag) {
    isTurning = true;
    isWalking = false;
    isButtonActionRunning = false;
    
    if (!rightTurnInitialized) {
      rightTurnSetup();
    }
    
    rightTurnLoop();
  }
  // ----- WALK FORWARD -----
  else if (walkForwardFlag) {
    isWalking = true;
    isTurning = false;
    isButtonActionRunning = false;
    walkForward();
  } 
  // ----- WALK BACKWARD -----
  else if (walkBackwardFlag) {
    isWalking = true;
    isTurning = false;
    isButtonActionRunning = false;
    walkBackward();
  } 
  // ----- BUTTON ACTION (Only runs when joystick is centered) -----
  else if (buttonPressedFlag) {
    // Run button action - it will handle its own looping and exit when button released
    performButtonAction();
  }
  // ----- STOP -----
  else {
    if (isWalking || isTurning || isButtonActionRunning) {
      resetToIdle();
    }
    setServoAngle(SERVO_5_CH, 100);
    currentAngles[SERVO_5_CH] = 100;
    delay(10);
  }
  
  delay(10);
}