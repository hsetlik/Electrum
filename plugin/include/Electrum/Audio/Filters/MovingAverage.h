#pragma once
#include "../AudioUtil.h"

#define MOVING_AVG_MAX 256

class MovingAvgRingBuf {
private:
  std::array<float, MOVING_AVG_MAX> buf = {};
  size_t head = 0;
  size_t length;

public:
  MovingAvgRingBuf(int size = 55) : length((size_t)size) {}
  void setSize(int val) {
    jassert(val < MOVING_AVG_MAX);
    length = (size_t)val;
    head = head % length;
  }
  int getSize() const { return (int)length; }
  float operator[](int index) {
    size_t idx = (size_t)index;
    return buf[(head + idx) % length];
  }
  void push(float val) {
    buf[head] = val;
    head = (head + 1) % length;
  }
};

class MovingAverage {
private:
  float sum = 0.0f;
  MovingAvgRingBuf buf;
  int currentSize;

public:
  MovingAverage(int size = 55);
  void setSize(int s);
  float process(float input);
};
