#ifndef HEADCONTROLLER_H
#define HEADCONTROLLER_H

#include <Arduino.h>
#include <Servo.h>
#include "motorcontroller.h";

struct SurroundingDistances {
  int right;
  int rightDiagonal;
  int center;
  int leftDiagonal;
  int left;
};

class HeadController {
public:
  HeadController();
  ~HeadController();
  void initHead(int trigger, int echo, int servo, int centerAngle);
  void setDebugMode(bool debugMode);
  int getDistanceAt(int headAngle);
  SurroundingDistances getSurroundingDistances();
private:
  bool isDebugMode;
  Servo servo;
  int trigger;
  int echo;
  int centerAngle;
  long microsecondsToCentimeters(int miliseconds);
  int distance();
};

#endif
