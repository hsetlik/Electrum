#include "ElectrumVoice.h"

ElectrumVoice::ElectrumVoice(EVT *tree, ModDestMap *map, int idx)
    : state(tree), modMap(map), index(idx), currentNote(-1),
      currentNoteVelocity(0.0f), gate(false), vge(this), filter(tree, idx),
      inQuickKill(false), queuedNote(0), queuedVelocity(0.0f) {
  for (int i = 0; i < NUM_OSCILLATORS; i++) {
    oscs.add(new WavetableOscillator(state, i));
  }
  for (int i = 0; i < NUM_ENVELOPES; i++) {
    envs.add(new AHDSREnvelope(state, i));
  }
}

bool ElectrumVoice::isBusy() { return gate || (!vge.isFinished()); }

void ElectrumVoice::startNote(int note, float vel) {
  currentNote = note;
  currentNoteVelocity = vel;
  gate = true;
  vge.start();
  for (auto e : envs) {
    e->gateStart();
  }
  state->startVoice(index);
}

void ElectrumVoice::stealNote(int note, float velocity) {
  inQuickKill = true;
  queuedNote = note;
  queuedVelocity = velocity;
  for (auto e : envs)
    e->killQuick();
  vge.killQuick();
}

void ElectrumVoice::stopNote() {
  vge.end();
  gate = false;
  for (auto e : envs) {
    e->gateEnd();
  }
}

float ElectrumVoice::filterSample(float input, bool updateDests) {

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

void ElectrumVoice::renderNextSample(float &left, float &right,
                                     bool updateDests) {
  if (!isBusy())
    return;
  // tick the modulation sources before we calculate any mod values
  //  note from benchmarking: the envelopes seem to take up about 25% of the
  //  time
  for (int i = 0; i < NUM_ENVELOPES; i++) {
    envs[i]->tick();
  }
  vge.tick();
  float output = 0.0f;
  for (auto o : oscs) {
    if (updateDests) {
      o->levelMod = getCurrentModDestValue(o->getLevelParamName());
      o->posMod = getCurrentModDestValue(o->getPosParamName());
      if (o->levelMod == 1.0f) {
      }
    }
    output +=
        o->getNextSample(Math::midiToHz(currentNote),
                         AudioSystem::getSampleRate(), o->posMod, o->levelMod);
  }
  // jassert(output <= 1.0f);
  output = filterSample(output, updateDests) * 0.5f * vge.getCurrentSample();
  left += output;
  right += output;
  // now we need to handle if this voice has been quick-killed and needs to
  // start the next note
  if (inQuickKill && vge.getCurrentSample() == 0.0f) {
    inQuickKill = false;
    startNote(queuedNote, queuedVelocity);
  }
}

void ElectrumVoice::updateForBlock() {
  for (auto o : oscs) {
    o->updateBasePos();
    o->updateBaseLevel();
  }
  baseFilterCutoff = state->getFloatParamValue(IDs::filterCutoff.toString());
  baseFilterRes = state->getFloatParamValue(IDs::filterResonance.toString());
  baseFilterMix = state->getFloatParamValue(IDs::filterMix.toString());
  baseFilterTracking =
      state->getFloatParamValue(IDs::filterTracking.toString());
  baseFilterType = state->getCurrentFilterType();
  // if this is currently the newest voice, update levels for the graphics side
  if (state->currentNewestVoice() == index && state->isEditorOpen()) {
    for (int i = 0; i < NUM_ENVELOPES; i++) {
      state->setLeadingVoiceEnvLevel(i, envs[i]->getCurrentSample());
    }
  }
}

float ElectrumVoice::getModValueForSample(const String &srcID) {
  if (srcID.contains(IDs::modWheelSource.toString())) {
    return state->getModWheel();
  } else if (srcID.contains(IDs::pitchWheelSource.toString())) {
    return state->getPitchBend();
  } else if (srcID.contains(IDs::perlinSource.toString())) {
    return state->perlinValue();
  } else if (srcID.contains(IDs::envSource.toString())) {
    int idx = srcID.getTrailingIntValue();
    return envs[idx]->getCurrentSample();
  } else {
    return 0.0f;
  }
}

float ElectrumVoice::getCurrentModDestValue(const String &destID) {
  auto pMap = modMap->getModsFor(destID);
  if (pMap == nullptr)
    return 0.0f;
  float value = 0.0f;
  auto &sources = *pMap;
  for (auto *src : sources) {
    value += getModValueForSample(src->sourceID) * src->depth;
  }
  return value;
}

//=========================================================================
VoiceGateEnvelope::VoiceGateEnvelope(ElectrumVoice *p)
    : parent(p), gate(false), forceKillQuick(false), lastOutput(0.0f) {}

void VoiceGateEnvelope::tick() {
  if (forceKillQuick) {
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

bool VoiceGateEnvelope::parentIsFinished() {
  if (gate)
    return false;
  for (auto e : parent->envs) {
    if (!e->isFinished())
      return false;
  }
  return true;
}
