#include "ElectrumVoice.h"

ElectrumVoice::ElectrumVoice(EVT *tree, ModDestMap *map, int idx)
    : state(tree), modMap(map), index(idx), currentNote(-1),
      currentNoteVelocity(0.0f), gate(false), vge(this), filter(tree, idx),
      inQuickKill(false), queuedNote(0), queuedVelocity(0.0f)
{
  for (int i = 0; i < NUM_OSCILLATORS; i++)
  {
    oscs.add(new WavetableOscillator(state, i));
  }
  for (int i = 0; i < NUM_ENVELOPES; i++)
  {
    envs.add(new AHDSREnvelope(state, i));
  }
}

bool ElectrumVoice::isBusy() { return gate || (!vge.isFinished()); }

void ElectrumVoice::startNote(int note, float vel)
{
  currentNote = note;
  currentNoteVelocity = vel;
  gate = true;
  vge.start();
  for (auto e : envs)
  {
    e->gateStart(vel);
  }
  state->startVoice(index);
}

void ElectrumVoice::stealNote(int note, float velocity)
{
  inQuickKill = true;
  queuedNote = note;
  queuedVelocity = velocity;
  for (auto e : envs)
    e->killQuick();
  vge.killQuick();
}

void ElectrumVoice::stopNote()
{
  vge.end();
  gate = false;
  for (auto e : envs)
  {
    e->gateEnd();
  }
}

float ElectrumVoice::filterSample(float input, bool updateDests)
{

  if (updateDests) // we try to avoid doing this every time
  {
    currentFilterMix =
        Math::bipolarFlerp(0.0f, 1.0f, baseFilterMix,
                           getCurrentModDestValue(IDs::filterMix.toString()));
    currentFilterTracking = Math::bipolarFlerp(
        0.0f, 1.0f, baseFilterTracking,
        getCurrentModDestValue(IDs::filterTracking.toString()));
    currentCutoff =
        ((float)Math::midiToHz(currentNote) * currentFilterTracking);
    currentCutoff = Math::bipolarFlerp(
        CUTOFF_HZ_MIN, CUTOFF_HZ_MAX, baseFilterCutoff + currentCutoff,
        getCurrentModDestValue(IDs::filterCutoff.toString()));
    currentRes = Math::bipolarFlerp(
        RESONANCE_MIN, RESONANCE_MAX, baseFilterRes,
        getCurrentModDestValue(IDs::filterResonance.toString()));
  }
  if (currentFilterMix == 0.0f)
    return input;
  float filtered =
      filter.process(input, baseFilterType, currentCutoff, currentRes);
  return Math::flerp(input, filtered, currentFilterMix);
}

void ElectrumVoice::filterSampleStereo(float &left, float &right,
                                       bool updateDests)
{
  if (updateDests) // we try to avoid doing this every time
  {
    currentFilterMix =
        Math::bipolarFlerp(0.0f, 1.0f, baseFilterMix,
                           getCurrentModDestValue(IDs::filterMix.toString()));
    currentFilterTracking = Math::bipolarFlerp(
        0.0f, 1.0f, baseFilterTracking,
        getCurrentModDestValue(IDs::filterTracking.toString()));
    currentCutoff =
        ((float)Math::midiToHz(currentNote) * currentFilterTracking);
    currentCutoff = Math::bipolarFlerp(
        CUTOFF_HZ_MIN, CUTOFF_HZ_MAX, baseFilterCutoff + currentCutoff,
        getCurrentModDestValue(IDs::filterCutoff.toString()));
    currentRes = Math::bipolarFlerp(
        RESONANCE_MIN, RESONANCE_MAX, baseFilterRes,
        getCurrentModDestValue(IDs::filterResonance.toString()));
  }
  if (currentFilterMix == 0.0f)
    return;
  filter.processStereo(left, right, baseFilterType, currentCutoff, currentRes,
                       currentFilterMix);
}

void ElectrumVoice::renderNextSample(float &left, float &right,
                                     bool updateDests)
{
  if (!isBusy())
    return;
  // tick the modulation sources before we calculate any mod values
  for (int i = 0; i < NUM_ENVELOPES; i++)
  {
    envs[i]->tick();
  }
  vge.tick();
  float voiceLeft = 0.0f;
  float voiceRight = 0.0f;
  for (uint8 i = 0; i < oscs.size(); i++)
  {
    if (updateDests)
    {
      oscState[i].levelMod =
          getCurrentModDestValue(IDs::oscillatorLevel.toString() + String(i));
      oscState[i].posMod =
          getCurrentModDestValue(IDs::oscillatorPos.toString() + String(i));
      oscState[i].panMod =
          getCurrentModDestValue(IDs::oscillatorPan.toString() + String(i));
      oscState[i].coarseMod = getCurrentModDestValue(
          IDs::oscillatorCoarseTune.toString() + String(i));
      oscState[i].fineMod = getCurrentModDestValue(
          IDs::oscillatorFineTune.toString() + String(i));
    }
    oscs[i]->renderSampleStereo(currentNote, AudioSystem::getSampleRate(),
                                oscState[i].levelMod, oscState[i].posMod,
                                oscState[i].panMod, oscState[i].coarseMod,
                                oscState[i].fineMod, voiceLeft, voiceRight);
  }
  // jassert(output <= 1.0f);
  filterSampleStereo(voiceLeft, voiceRight, updateDests);
  left += voiceLeft * vge.getCurrentSample();
  right += voiceRight * vge.getCurrentSample();
  // now we need to handle if this voice has been quick-killed and needs to
  // start the next note
  if (inQuickKill && vge.getCurrentSample() == 0.0f)
  {
    inQuickKill = false;
    startNote(queuedNote, queuedVelocity);
  }
}

void ElectrumVoice::updateForBlock()
{
  for (auto o : oscs)
  {
    o->updateBasePos();
    o->updateBaseLevel();
    o->updateBasePan();
    o->updateBaseCoarse();
    o->updateBaseFine();
  }
  baseFilterCutoff = state->getFloatParamValue(IDs::filterCutoff.toString());
  baseFilterRes = state->getFloatParamValue(IDs::filterResonance.toString());
  baseFilterMix = state->getFloatParamValue(IDs::filterMix.toString());
  baseFilterTracking =
      state->getFloatParamValue(IDs::filterTracking.toString());
  baseFilterType = state->getCurrentFilterType();
  // if this is currently the newest voice, update levels for the graphics side
  if (state->currentNewestVoice() == index && state->isEditorOpen())
  {
    for (int i = 0; i < NUM_ENVELOPES; i++)
    {
      state->setLeadingVoiceEnvLevel(i, envs[i]->getCurrentSample());
    }
    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
      state->setLeadingVoiceOscPosition(i, oscs[i]->getLastPosition());
    }
  }
}

float ElectrumVoice::getModValueForSample(const String &srcID)
{
  if (srcID.contains(IDs::modWheelSource.toString()))
  {
    return state->getModWheel();
  } else if (srcID.contains(IDs::pitchWheelSource.toString()))
  {
    return state->getPitchBend();
  } else if (srcID.contains(IDs::perlinSource.toString()))
  {
    return state->perlinValue();
  } else if (srcID.contains("envSource"))
  {
    int idx = srcID.getTrailingIntValue();
    return envs[idx]->getCurrentSample();
  } else
  {
    return 0.0f;
  }
}

float ElectrumVoice::getCurrentModDestValue(const String &destID)
{
  auto pMap = modMap->getModsFor(destID);
  if (pMap == nullptr)
  {
    DLog::log("No modulations found for ID " + destID);
    return 0.0f;
  }
  float value = 0.0f;
  auto &sources = *pMap;
  for (auto *src : sources)
    value += getModValueForSample(src->sourceID) * src->depth;
  return value;
}

//=========================================================================
VoiceGateEnvelope::VoiceGateEnvelope(ElectrumVoice *p)
    : parent(p), gate(false), forceKillQuick(false), lastOutput(0.0f)
{
}

void VoiceGateEnvelope::tick()
{
  if (forceKillQuick)
  {
    lastOutput = std::max(lastOutput - levelDelta(), 0.0f);
    forceKillQuick = lastOutput > 0.0f;
  } else if (gate)
    lastOutput = std::min(lastOutput + levelDelta(), 1.0f);
  else if (!parentIsFinished())
    lastOutput = 1.0f;
  else
    lastOutput = std::max(lastOutput - levelDelta(), 0.0f);
}

void VoiceGateEnvelope::start() { gate = true; }

bool VoiceGateEnvelope::parentIsFinished()
{
  if (gate)
    return false;
  for (auto e : parent->envs)
  {
    if (!e->isFinished())
      return false;
  }
  return true;
}
