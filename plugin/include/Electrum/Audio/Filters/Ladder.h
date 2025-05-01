#pragma once

#include "TPTFilter.h"

class LadderLPBasic {
private:
  // 2-D array for the filter state variables
  // accessed like [channel][pole]
  float zState[2][4];

  // the cutoff variables for each stage
  float cutoffHz = 2000.0f;
  float g4;
  float bigG;
  float g3;
  float g2;
  float g;

  float k = 3.0f;
  void updateG(double sampleRate);

public:
  LadderLPBasic();
  void prepare(double sampleRate) { updateG(sampleRate); }
  // getters
  float getCutoffHz() const { return cutoffHz; }
  float getResonance() const { return k; }
  // setters
  void setCutoffHz(float hz);
  void setResonance(float val) { k = val * 4.0f; }
  // main processing
  float processMono(float input, int channel = 0);
  void processStereo(float& left, float& right);
};

//==============================

#define LADDER_MAX_K 6.5f

class LadderLP {
private:
  float zState[2][4];

  // the cutoff variables
  float cutoffHz = 2000.0f;
  float g4;
  float bigG;
  float g3;
  float g2;
  float g;

  float k = 3.0f;
  void updateG(double sampleRate);

public:
  LadderLP();
  void prepare(double sampleRate) { updateG(sampleRate); }
  // getters
  float getCutoffHz() const { return cutoffHz; }
  float getResonance() const { return k; }
  // setters
  void setCutoffHz(float hz);
  void setResonance(float val) { k = val * LADDER_MAX_K; }
  // main processing
  float processMono(float input, int channel = 0);
  void processStereo(float& left, float& right);
};

//----------------------------

class LadderHighPass {
private:
  float zState[2][4];

  // the cutoff variables
  float cutoffHz = 2000.0f;
  float g4;
  float bigG;
  float g3;
  float g2;
  float g;

  float k = 3.0f;
  void updateG(double sampleRate);

public:
  LadderHighPass();
  void prepare(double sampleRate) { updateG(sampleRate); }
  // getters
  float getCutoffHz() const { return cutoffHz; }
  float getResonance() const { return k; }
  // setters
  void setCutoffHz(float hz);
  void setResonance(float val) { k = val * LADDER_MAX_K; }
  // main processing
  float processMono(float input, int channel = 0);
  void processStereo(float& left, float& right);
};
