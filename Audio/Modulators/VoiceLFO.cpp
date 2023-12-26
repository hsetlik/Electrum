#include "LFO.h"
#include "VoiceLFO.h"
#include <cmath>

VoiceLFO::VoiceLFO(EVT *tree, int idx)
    : state(tree), sampleRate((float)AudioSystem::getSampleRate()), lastOutput(0.0f),
      currentPhase(0.0f), inQuickKill(false), quickKillDelta(0.0f), index(idx)
{
}

void VoiceLFO::startQuickKill()
{
  inQuickKill = true;
  quickKillDelta = lastOutput / ((QUICK_KILL_MS / 1000.0f) * sampleRate);
}

float VoiceLFO::getNextSample()
{
  auto lfo = state->getAudioData()->getLFOData(index);
  float phaseInc = lfo->frequency / (float)AudioSystem::getSampleRate();
  currentPhase = std::fmod(currentPhase + phaseInc, 1.0f);
  return LFOFunctions::curveLFO(lfo->center, lfo->curveA, lfo->curveB, currentPhase);
}

void VoiceLFO::tick()
{
  if (inQuickKill && lastOutput > 0.0f)
  {
    lastOutput = lastOutput - quickKillDelta;
    if (lastOutput <= 0.0f)
    {
      lastOutput = 0.0f;
      inQuickKill = false;
    }
  } else
  {
    lastOutput = getNextSample();
  }
}
