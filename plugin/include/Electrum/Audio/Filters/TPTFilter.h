#pragma once
#include "../AudioUtil.h"

/* This is a first order TPT filter
 * based on the description in Vadim
 * Zavalishin's "The art of VA filter design":
 * https://www.native-instruments.com/fileadmin/ni_media/downloads/pdf/VAFilterDesign_1.1.1.pdf?srsltid=AfmBOoom5Zp92JEhD7bAceeb-pGWZfT996jMp8k3oQQYerF0I36QvayA
 * */

enum TPTType { lowpass, highpass, allpass };

class TPTFilter {
private:
  TPTType type = TPTType::lowpass;
  // equivalent to 'g / (1 + g)' in the equations
  // on p. 48
  float bigG;
  // our cutoff freq
  float cutoffHz = 2000.0f;
  // our delayed samples for two stereo channels
  float z1[2];
  void updateG(double sampleRate);

public:
  TPTFilter();
  void prepare(double sampleRate) { updateG(sampleRate); }
  // param setters/getters
  void setFrequency(float hz);
  void setType(TPTType t);
  float getFrequency() const { return cutoffHz; }
  TPTType getType() const { return type; }
  // process a single sample in mono
  float processMono(float input, int channel = 0);
  // process a pair of stereo samples by reference
  void processStereo(float& left, float& right);
};
