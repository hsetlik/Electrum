#include "Electrum/Audio/Filters/MovingAverage.h"

//===================================================
MovingAverage::MovingAverage(int size) : buf(size) {}

float MovingAverage::process(float input) {
  // 1. save the value of the oldest sample in the
  //  buffer before we overwrite it, then update the buffer
  const float oldest = buf[0];
  buf.push(input);
  // 2. subtract the oldest sample from the running sum
  sum -= oldest;
  // 3. add the newest sample
  sum += input;
  // 4. divide for the new mean
  return sum / (float)currentSize;
}

void MovingAverage::setSize(int s) {
  currentSize = s;
  buf.setSize(currentSize);
}
