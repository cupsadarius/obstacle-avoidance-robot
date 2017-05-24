#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include <Arduino.h>

struct MotorPinout {
  int positive;
  int negative;
};

class MotorController {
public:
  MotorController();
  ~MotorController();
  void setDebugMode(bool debugMode);
  void initMotors(MotorPinout leftSide, MotorPinout rightSide);
  void forward(int duration);
  void backward(int duration);
  void left(int duration);
  void right(int duration);
  void stop();
private:
  bool isDebugMode;
  MotorPinout leftSide;
  MotorPinout rightSide;
};

#endif
