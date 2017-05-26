#include "motorcontroller.h";

//<<constructor>>
MotorController::MotorController(){}

//<<destructor>>
MotorController::~MotorController(){}

void MotorController::initMotors(MotorPinout leftSide, MotorPinout rightSide) {
  MotorController::leftSide = leftSide;
  MotorController::rightSide = rightSide;
  pinMode(leftSide.positive, OUTPUT);
  pinMode(leftSide.negative, OUTPUT);
  pinMode(rightSide.positive, OUTPUT);
  pinMode(rightSide.negative, OUTPUT);
  digitalWrite(MotorController::leftSide.positive, LOW);
  digitalWrite(MotorController::leftSide.negative, LOW);
  digitalWrite(MotorController::rightSide.positive, LOW);
  digitalWrite(MotorController::rightSide.negative, LOW);
}

void MotorController::setDebugMode(bool debugMode) {
  MotorController::isDebugMode = debugMode;
}

void MotorController::right(int duration) {
  if (MotorController::isDebugMode) {
    return;
  }
  digitalWrite(MotorController::leftSide.positive, HIGH);
  digitalWrite(MotorController::leftSide.negative, LOW);
  digitalWrite(MotorController::rightSide.positive, HIGH);
  digitalWrite(MotorController::rightSide.negative, LOW);
  delay(duration);
}

void MotorController::left(int duration) {
  if (MotorController::isDebugMode) {
    return;
  }
  digitalWrite(MotorController::leftSide.positive , LOW);
  digitalWrite(MotorController::leftSide.negative, HIGH);
  digitalWrite(MotorController::rightSide.positive, LOW);
  digitalWrite(MotorController::rightSide.negative, HIGH);
  delay(duration);
}

void MotorController::backward(int duration) {
  if (MotorController::isDebugMode) {
    return;
  }
  digitalWrite (MotorController::leftSide.positive, LOW);
  digitalWrite (MotorController::leftSide.negative, HIGH);
  digitalWrite (MotorController::rightSide.positive, HIGH);
  digitalWrite (MotorController::rightSide.negative, LOW);
  delay(duration ? duration : 0);
}

void MotorController::forward(int duration) {
  if (MotorController::isDebugMode) {
    return;
  }
  digitalWrite (MotorController::leftSide.positive, HIGH);
  digitalWrite (MotorController::leftSide.negative, LOW);
  digitalWrite (MotorController::rightSide.positive, LOW);
  digitalWrite (MotorController::rightSide.negative, HIGH);
  delay(duration ? duration : 0);
}

void MotorController::stop() {
  if (MotorController::isDebugMode) {
    return;
  }
  digitalWrite (MotorController::leftSide.positive , LOW);
  digitalWrite (MotorController::leftSide.negative, LOW);
  digitalWrite (MotorController::rightSide.positive, LOW);
  digitalWrite (MotorController::rightSide.negative, LOW);
}
