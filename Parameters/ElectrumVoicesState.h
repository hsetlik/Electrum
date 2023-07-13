#pragma once
#include "Identifiers.h"

//the idea here is that we use the 32 bits of an int to keep track of which voices are on/off

namespace ElectrumVoicesState
{
  int startVoice(int data, int idx);
  int endVoice(int data, int idx);
  bool isVoiceActive(int data, int idx);
  int numActiveVoices(int data);
}
