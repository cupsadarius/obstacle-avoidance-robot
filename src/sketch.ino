#include "IRremote.h"
#include "motorcontroller.h";
#include "headcontroller.h";

const bool debugMode = false;

//Pins for ultrasonic sensor
const int trigger = 10;
const int echo = 11;

MotorController motorController;
HeadController headController;

//Pin for IR control
int receiver = 12; // pin 1 of IR receiver to Arduino digital pin 12
IRrecv irrecv(receiver);           // create instance of 'irrecv'
decode_results results;
char contCommand;
int modeControl = 0;
int power = 0;

const int distanceLimit = debugMode ? 15 : 35; //Distance limit for obstacles in front
const int sideDistanceLimit = debugMode ? 10 : 30; //Minimum distance in cm to obstacles at both sides (the robot will allow a shorter distance sideways)

int distance;
int cycles = 0;
int turnDirection; //Gets 'l', 'r' or 'f' depending on which direction is obstacle free
const int turnTime = 62; //Time the robot spends turning (miliseconds)
int thereIs;
Servo head;
const int headDelay = debugMode ? 300 : 100;

const int BACKWARDS = 0;
const int LEFT = 1;
const int LEFT_DIAGONAL = 2;
const int CENTER = 3;
const int RIGHT_DIAGONAL = 4;
const int RIGHT = 5;

void setup() {
  struct MotorPinout leftSide, rightSide;
  leftSide.positive = 6;
  leftSide.negative = 7;
  rightSide.positive = 8;
  rightSide.negative = 9;
  motorController.initMotors(leftSide, rightSide);
  motorController.setDebugMode(debugMode);
  headController.initHead(10, 11, 5, 60);
  headController.setDebugMode(debugMode);
  irrecv.enableIRIn(); // Start the IR receiver
  Serial.begin(9600);
}

int decideNew() {
  struct SurroundingDistances distances = headController.getSurroundingDistances();
  printDistances(distances);
  if (
    distances.center > distanceLimit &&
    distances.rightDiagonal >= sideDistanceLimit &&
    distances.leftDiagonal >= sideDistanceLimit
  ) {
    return CENTER;
  }

  if (
    distances.right > sideDistanceLimit &&
    distances.rightDiagonal >= sideDistanceLimit &&
    distances.center <= distanceLimit ||
    distances.center <= distanceLimit &&
    distances.left > sideDistanceLimit &&
    distances.leftDiagonal >= sideDistanceLimit
  ) {

    return distances.right > distances.left ? RIGHT : LEFT;
  }

  if (
    distances.rightDiagonal > sideDistanceLimit &&
    distances.right >= sideDistanceLimit &&
    distances.center <= distanceLimit ||
    distances.center <= distanceLimit &&
    distances.left >= sideDistanceLimit &&
    distances.leftDiagonal > sideDistanceLimit
  ) {
    return distances.rightDiagonal > distances.leftDiagonal ? RIGHT_DIAGONAL : LEFT_DIAGONAL;
  }

  return BACKWARDS;
}

// int decide() {
//   watchSurrounding();
//   printDistances();
//   if (
//     rightScanVal < sideDistanceLimit &&
//     rDiagonalScanVal < sideDistanceLimit &&
//     leftScanVal < sideDistanceLimit &&
//     lDiagonalScanVal < sideDistanceLimit &&
//     centerScanVal < distanceLimit
//   ) {
//     return BACKWARDS; // turn arround
//   }
//
//   if (
//     leftScanVal > rightScanVal &&
//     leftScanVal > lDiagonalScanVal &&
//     leftScanVal > centerScanVal &&
//     leftScanVal > rDiagonalScanVal &&
//     centerScanVal < distanceLimit &&
//     leftScanVal > sideDistanceLimit
//   ) {
//     return LEFT; // left
//   }
//
//   if (
//     lDiagonalScanVal > rightScanVal &&
//     lDiagonalScanVal >= leftScanVal &&
//     lDiagonalScanVal > centerScanVal &&
//     lDiagonalScanVal > rDiagonalScanVal &&
//     centerScanVal < distanceLimit &&
//     lDiagonalScanVal > sideDistanceLimit
//   ) {
//     return LEFT_DIAGONAL; // left diagonal
//   }
//
//   if (
//     rDiagonalScanVal >= rightScanVal &&
//     rDiagonalScanVal > leftScanVal &&
//     rDiagonalScanVal > centerScanVal &&
//     rDiagonalScanVal > lDiagonalScanVal &&
//     centerScanVal < distanceLimit &&
//     rDiagonalScanVal > sideDistanceLimit
//   ) {
//     return RIGHT_DIAGONAL; // right diagonal
//   }
//
//   if (
//     rightScanVal > leftScanVal &&
//     rightScanVal > lDiagonalScanVal &&
//     rightScanVal > centerScanVal &&
//     rightScanVal > rDiagonalScanVal &&
//     centerScanVal < distanceLimit &&
//     rightScanVal > sideDistanceLimit
//   ) {
//     return RIGHT; // right
//   }
//
//   return CENTER; //front
// }

void translateIR() { //Used when robot is switched to operate in remote control mode
  switch (results.value) {
    case 0xFF629D: //Case 'FORWARD'
      motorController.forward(0);
      break;
    case 0xFF22DD: //Case 'LEFT'
      motorController.left(turnTime * 4);
      motorController.stop();
      break;
    case 0xFF02FD: //Case 'OK'
      motorController.stop();
      break;
    case 0xFFC23D: //Case 'RIGHT'
      motorController.right(turnTime * 4);
      motorController.stop();
      break;
    case 0xFFA857: //Case 'REVERSE'
      motorController.backward(0);
      break;
    case 0xFF42BD:  //Case '*'
      modeControl = 0; motorController.stop(); // If an '*' is received, switch to automatic robot operating mode
      break;
    default:
      ;
  }// End Case
  delay(100); // Do not get immediate repeat
}

void loop() {
  if (irrecv.decode(&results)) { //Check if the remote control is sending a signal
    if (results.value == 0xFF6897) { //If an '1' is received, turn on robot
      power = 1;
    }
    if (results.value == 0xFF4AB5) { //If a '0' is received, turn off robot
      motorController.stop();
      power = 0;
    }
    if (results.value == 0xFF42BD) { //If an '*' is received, switch operating mode from automatic robot to remote control (press also "*" to return to automatic robot mode)
      modeControl = 1; //  Activate remote control operating mode
      motorController.stop(); //The robot stops and starts responding to the user's directions
    }
    printRemoteKeys();
    irrecv.resume(); // receive the next value
  }
  while (modeControl == 1) { //The system gets into this loop during the remote control mode until modecontrol=0 (with '*')
    if (irrecv.decode(&results)) { //If something is being received
      translateIR();//Do something depending on the signal received
      irrecv.resume(); // receive the next value
    }
  }
  if (power == 1) {
    motorController.forward(0);  // if nothing is wrong go forward using go() function above.
    if (cycles >= 50) {
      printDecisions();
      changeDirection();
      cycles = 0;
    } else {
      ++cycles;
    }

    distance = headController.getDistanceAt(60); // use the watch() function to see if anything is ahead (when the robot is just moving forward and not looking around it will test the distance in front)
    if (distance < distanceLimit) { // The robot will just stop if it is completely sure there's an obstacle ahead (must test 10 times) (needed to ignore ultrasonic sensor's false signals)
      ++thereIs;
    }
    if (distance > distanceLimit) {
      thereIs = 0;
    } //Count is restarted
    if (thereIs > 10) { // filter noise
      motorController.stop(); // Since something is ahead, stop moving.
      printDecisions();
      changeDirection();
      thereIs = 0;
    }
  }
}

void changeDirection() {
  motorController.stop();
  turnDirection = decideNew(); //Decide which direction to turn.
  switch (turnDirection) {
    case LEFT:
      motorController.left(turnTime * 4);
      break;
    case LEFT_DIAGONAL:
      motorController.left(turnTime * 2);
      break;
    case RIGHT:
      motorController.right(turnTime * 4);
      break;
    case RIGHT_DIAGONAL:
      motorController.right(turnTime * 2);
      break;
    case BACKWARDS:
      motorController.backward(turnTime * 2);
      motorController.left(turnTime * 8);
      break;
    case CENTER:
      ; //Do not turn if there was actually nothing ahead
      break;
  }
}

void printDistances(SurroundingDistances surroundingDistances) {
  Serial.print("Left distance: ");
  Serial.print(surroundingDistances.left);
  Serial.print(" Left diagonal distance: ");
  Serial.print(surroundingDistances.leftDiagonal);
  Serial.print(" Center distance: ");
  Serial.print(surroundingDistances.center);
  Serial.print(" Right diagonal distance: ");
  Serial.print(surroundingDistances.rightDiagonal);
  Serial.print(" Right distance: ");
  Serial.println(surroundingDistances.right);
}

void printDecisions() {
  Serial.print("Distance to obstacle ");
  Serial.print(distance);
  Serial.print(" and it's been there for ");
  Serial.print(thereIs);
  Serial.print(" checks, decided to ");
  switch (turnDirection) {
    case LEFT:
      Serial.println(" turn left");
      break;
    case LEFT_DIAGONAL:
      Serial.println(" turn left diagonal");
      break;
    case RIGHT:
      Serial.println(" turn right");
      break;
    case RIGHT_DIAGONAL:
      Serial.println(" turn right diagonal");
      break;
    case BACKWARDS:
      Serial.println(" turn backwards");
      break;
    case CENTER:
      Serial.println(" not turn");
      ; //Do not turn if there was actually nothing ahead
      break;
  }
}

void printRemoteKeys() {
  Serial.print("Remote key pressed: ");
  switch (results.value) {
    case 0xFF629D: //Case 'FORWARD'
      Serial.println("FORWARD");
      break;
    case 0xFF22DD: //Case 'LEFT'
      Serial.println("LEFT");
      motorController.stop();
      break;
    case 0xFF02FD: //Case 'OK'
      Serial.println("STOP / OK");
      break;
    case 0xFFC23D: //Case 'RIGHT'
      Serial.println("RIGHT");
      motorController.stop();
      break;
    case 0xFFA857: //Case 'REVERSE'
      Serial.println("REVERSE");
      break;
    case 0xFF42BD:  //Case '*'
      Serial.println("Manual mode");
      break;
    case 0xFF6897:
      Serial.println("Start robot");
      break;
    case 0xFF4AB5:
      Serial.println("Stop robot");
      break;
    default:
      ;
  }// End Case
}