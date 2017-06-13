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

long HeadController::microsecondsToCentimeters(long miliseconds) {
  return miliseconds / 29 / 2;
}

int HeadController::distance() {
  long responseTime;
  digitalWrite(HeadController::trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(HeadController::trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(HeadController::trigger, LOW);
  responseTime = pulseIn(HeadController::echo, HIGH);
  delayMicroseconds(5);

  if (HeadController::isDebugMode) {
    Serial.print(" is ");
    Serial.print(HeadController::microsecondsToCentimeters(responseTime));
    Serial.print(" response time is ");
    Serial.println(responseTime);
  }

  return round(HeadController::microsecondsToCentimeters(responseTime));
}

int filterSpikes(int data[], int length, int treshold = 5) {
  int medianValue = 0;
  int filtered = 0;
  for (int i = 0; i < length; i++) {
    medianValue += data[i];
  }
  medianValue = medianValue / length;
  int j = 0;
  for (int i = 0; i < length; i++) {
    if (data[i] - treshold <= medianValue) {
      filtered += data[i];
      j++;
    }
  }
  Serial.println(medianValue);
  Serial.println(j);
  return filtered / j;
}

int improvedFilterSpikes(int data[], int length) {
  int size = length;
  int min = data[0];
  int max = data[0];
  int minPos = 0;
  int maxPos = 0;
  for (int i = 1; i < length; i++) {
    if (data[i] < min) {
      min = data[i];
      minPos = i;
    }
    if (data[i] > max) {
      max = data[i];
      maxPos = i;
    }
  }

  memmove(data + minPos, data + minPos + 1, (size - 2 - 1) * sizeof(int));
  size--;
  memmove(data + maxPos, data + maxPos + 1, (size - 2 - 1) * sizeof(int));
  size--;
  return filterSpikes(data, length - 2);
}

int HeadController::getDistanceAt(int headAngle) {
  HeadController::servo.write(headAngle);
  if (HeadController::isDebugMode) {
    Serial.print("Distance at angle ");
    Serial.println(headAngle);
  }
  int distance[5];
  for (int i = 0; i < 5; i++) {
    distance[i] = HeadController::distance();
  }
  int filtered = improvedFilterSpikes(distance, 5);
  Serial.println(filtered);
  return filtered;
}

struct SurroundingDistances HeadController::getSurroundingDistances() {
  struct SurroundingDistances surroundingDistances;
  int headDelay = HeadController::isDebugMode ? 500 : 150;
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
