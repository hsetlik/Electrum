#ifndef VOICESSTATE_CPP
#define VOICESSTATE_CPP
#include "ElectrumVoicesState.h"

int ElectrumVoicesState::startVoice(int data, int idx)
{
  return data | (1 << idx);
}

int ElectrumVoicesState::endVoice(int data, int idx)
{
  return data & ~(1 << idx);
}
  
bool ElectrumVoicesState::isVoiceActive(int data, int idx)
{
  return data & (1 << idx);
}


int ElectrumVoicesState::numActiveVoices(int data)
{
  int value = 0;
  for(int i = 0; i < (int)sizeof(int); i++)
  {
    
  }
  return value;
}
#endif
