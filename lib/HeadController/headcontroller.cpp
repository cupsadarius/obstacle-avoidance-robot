#include "headcontroller.h";

HeadController::HeadController() {}

HeadController::~HeadController() {}

void HeadController::initHead(int trigger, int echo, int servo, int centerAngle) {
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  HeadController::servo.attach(servo);
  HeadController::servo.write(centerAngle);
  HeadController::trigger = trigger;
  HeadController::echo = echo;
  HeadController::centerAngle = centerAngle;
  digitalWrite(trigger, LOW);
}

void HeadController::setDebugMode(bool debugMode) {
  HeadController::isDebugMode = debugMode;
}

long HeadController::microsecondsToCentimeters(int miliseconds) {
  return miliseconds / 29 / 2;
}

int HeadController::distance() {
  long responseTime;
  digitalWrite(HeadController::trigger, LOW);
  delayMicroseconds(5);
  digitalWrite(HeadController::trigger, HIGH);
  delayMicroseconds(15);
  digitalWrite(HeadController::trigger, LOW);
  responseTime = pulseIn(HeadController::echo, HIGH);
  return round(HeadController::microsecondsToCentimeters(responseTime));
}

int HeadController::getDistanceAt(int headAngle) {
  HeadController::servo.write(headAngle);
  return HeadController::distance();
}

struct SurroundingDistances HeadController::getSurroundingDistances() {
  struct SurroundingDistances surroundingDistances;
  int headDelay = HeadController::isDebugMode ? 1000 : 100;
  int increment = round(HeadController::centerAngle * 2 / 4);
  int rightAngle = 0;
  int rightDiagonalAngle = rightAngle + increment;
  int centerAngle = HeadController::centerAngle;
  int leftDiagonalAngle = centerAngle + increment;
  int leftAngle = leftDiagonalAngle + increment;
  surroundingDistances.right = HeadController::getDistanceAt(rightAngle);
  delay(headDelay);
  surroundingDistances.rightDiagonal = HeadController::getDistanceAt(rightDiagonalAngle);
  delay(headDelay);
  surroundingDistances.center = HeadController::getDistanceAt(centerAngle);
  delay(headDelay);
  surroundingDistances.leftDiagonal = HeadController::getDistanceAt(leftDiagonalAngle);
  delay(headDelay);
  surroundingDistances.left = HeadController::getDistanceAt(leftAngle);
  delay(headDelay);
  HeadController::getDistanceAt(centerAngle);

  return surroundingDistances;
}
