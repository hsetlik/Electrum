#pragma once

#include "TPTFilter.h"

class TPTLadder {
private:
  // 2-D array for the filter state variables
  // accessed like [channel][pole]
  float zState[2][4];

  // the cutoff variables for each stage
  float cutoffHz = 2000.0f;
  float bigG;
  float bigGFilter;
  float g3;
  float g2;
  float g;

  float k;
  void updateG(double sampleRate);

public:
  TPTLadder();
  void prepare(double sampleRate) { updateG(sampleRate); }
  // getters
  float getCutoffHz() const { return cutoffHz; }
  float getResonance() const { return k; }
  // setters
  void setCutoffHz(float hz);
  void setResonance(float val) { k = val; }
  // main processing
  float processMono(float input, int channel = 0);
  void processStereo(float& left, float& right);
};
