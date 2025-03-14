#pragma once
#include "Identifiers.h"

namespace AudioSystem {

double getSampleRate();
int getBlockSize();
size_t getNumChannels();

void setSampleRate(double rate);
void setBlockSize(int size);
void setNumChannels(size_t num);
} // namespace AudioSystem
