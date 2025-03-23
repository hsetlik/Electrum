#include "Electrum/Audio/Synth/Voice.h"
#include "Electrum/Audio/Modulator/AHDSR.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"

float AudioSourceState::baseValueForModDest(int destID) const {
  // check if it's an oscillator
  if (destID <= ModDestE::osc3Pan) {
    const int oscID = destID / 5;
    const int pID = destID % 5;
    switch (pID) {
      case 0:
        return wOsc[oscID].getCoarse();
      case 1:
        return wOsc[oscID].getFine();
      case 2:
        return wOsc[oscID].getPos();
      case 3:
        return wOsc[oscID].getLevel();
      default:
        return wOsc[oscID].getPan();
    }
  }
  jassert(false);
  return 0.0f;
}

void AudioSourceState::updateForBlock(ElectrumState* tree) {
  // oscillators----------------------------------
  for (int i = 0; i < NUM_OSCILLATORS; ++i) {
    String iStr(i);
    // 1. figure out the IDs
    const String posID = ID::oscillatorPos.toString() + iStr;
    const String levelID = ID::oscillatorLevel.toString() + iStr;
    const String coarseID = ID::oscillatorCoarseTune.toString() + iStr;
    const String fineID = ID::oscillatorFineTune.toString() + iStr;
    const String panID = ID::oscillatorPan.toString() + iStr;
    // 2. grab from the atomic values
    const float _pos = tree->getRawParameterValue(posID)->load();
    const float _level = tree->getRawParameterValue(levelID)->load();
    const float _coarse = tree->getRawParameterValue(coarseID)->load();
    const float _fine = tree->getRawParameterValue(fineID)->load();
    const float _pan = tree->getRawParameterValue(panID)->load();
    // 3. assign to the DSP objects
    wOsc[i].setPos(_pos);
    wOsc[i].setLevel(_level);
    wOsc[i].setCoarse(_coarse);
    wOsc[i].setFine(_fine);
    wOsc[i].setPan(_pan);
  }
  // envelopes----------------------------------
  for (int i = 0; i < NUM_ENVELOPES; ++i) {
    String iStr(i);
    const String aMsID = ID::attackMs.toString() + iStr;
    const String aCurveID = ID::attackCurve.toString() + iStr;
    const String hID = ID::holdMs.toString() + iStr;
    const String dMsID = ID::decayMs.toString() + iStr;
    const String dCurveID = ID::decayCurve.toString() + iStr;
    const String sID = ID::sustainLevel.toString() + iStr;
    const String rMsID = ID::releaseMs.toString() + iStr;
    const String rCurveID = ID::releaseCurve.toString() + iStr;
    const String vID = ID::velocityTracking.toString() + iStr;

    ahdsr_data_t envParams;
    envParams.attackMs = tree->getRawParameterValue(aMsID)->load();
    envParams.attackCurve = tree->getRawParameterValue(aCurveID)->load();
    envParams.holdMs = tree->getRawParameterValue(hID)->load();
    envParams.decayMs = tree->getRawParameterValue(dMsID)->load();
    envParams.decayCurve = tree->getRawParameterValue(dCurveID)->load();
    envParams.sustainLevel = tree->getRawParameterValue(sID)->load();
    envParams.velTracking = tree->getRawParameterValue(vID)->load();
    envParams.releaseMs = tree->getRawParameterValue(rMsID)->load();
    envParams.releaseCurve = tree->getRawParameterValue(rCurveID)->load();

    env[i].updateState(envParams);
  }
}

//===================================================

VoiceGateEnvelope::VoiceGateEnvelope(ElectrumVoice* p)
    : parent(p), gate(false), forceKillQuick(false), lastOutput(0.0f) {}

void VoiceGateEnvelope::tick() {
  const float ld = levelDelta();
  if (forceKillQuick) {
    lastOutput = std::max(lastOutput - ld, 0.0f);
    forceKillQuick = lastOutput > 0.0f;
  } else if (gate)
    lastOutput = std::min(lastOutput + ld, 1.0f);
  else if (!parentIsFinished())
    lastOutput = 1.0f;
  else
    lastOutput = std::max(lastOutput - ld, 0.0f);
}

void VoiceGateEnvelope::start() {
  gate = true;
}

bool VoiceGateEnvelope::parentIsFinished() {
  if (gate)
    return false;
  for (auto e : parent->envs) {
    if (!e->isFinished())
      return false;
  }
  return true;
}

//===================================================
//
// the input value coresponds to the 'mod_source_t' enum
// in ElectrumState.h
float ElectrumVoice::_currentModSrcVal(int src) {
  const ModSourceE id = (ModSourceE)src;
  if (id < ModSourceE::ModWheel) {
    return envs[src]->getCurrentSample();
  }
  // TODO: other modulation sources get implemented here
  else
    return 0.0f;
}

float ElectrumVoice::_normalizedModulationForDest(ModMap* map, int destID) {
  // this static array holds info about the current modulation dest
  static mod_src_t currentMods[MOD_SOURCES];
  int numSources = 0;
  float sum = 0.0f;
  map->getSourcesSafe(currentMods, &numSources, destID);
  // return 0 if this dest isn't used
  if (numSources < 1)
    return sum;
  // otherwise add each of the used sources
  // to the sum
  for (int i = 0; i < numSources; ++i) {
    sum += _currentModSrcVal(currentMods[i].source) * currentMods[i].depth;
  }
  return std::clamp(sum, -1.0f, 1.0f);
}

void ElectrumVoice::_updateModDests(ModMap* map) {
  // 1. Update the  oscillators
  int oscIdx = 0;
  int destIdx = 0;
  while (oscIdx < NUM_OSCILLATORS && destIdx < MOD_DESTS) {
    oscModState[oscIdx].coarseMod = _normalizedModulationForDest(map, destIdx);
    ++destIdx;
    oscModState[oscIdx].fineMod = _normalizedModulationForDest(map, destIdx);
    ++destIdx;
    oscModState[oscIdx].posMod = _normalizedModulationForDest(map, destIdx);
    ++destIdx;
    oscModState[oscIdx].levelMod = _normalizedModulationForDest(map, destIdx);
    ++destIdx;
    oscModState[oscIdx].panMod = _normalizedModulationForDest(map, destIdx);
    ++destIdx;
    ++oscIdx;
  }
}

//===================================================
ElectrumVoice::ElectrumVoice(ElectrumState* s, AudioSourceState* a, int idx)
    : state(s), audioState(a), vge(this), voiceIndex(idx) {
  // instantiate the oscillators
  for (int i = 0; i < NUM_OSCILLATORS; i++) {
    oscs.add(new WavetableOscillator(&audioState->wOsc[i], i));
  }
  // instantiate the envelopes
  for (int i = 0; i < NUM_ENVELOPES; ++i) {
    envs.add(new AHDSREnvelope(&audioState->env[i], i));
  }
}

bool ElectrumVoice::isBusy() {
  return gate || (!vge.isFinished());
}

void ElectrumVoice::startNote(int note, float vel) {
  currentNote = note;
  currentNoteVelocity = vel;
  gate = true;
  vge.start();
  for (auto* e : envs) {
    e->gateStart(vel);
  }
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

void ElectrumVoice::sampleRateSet(double sr) {
  for (auto* e : envs) {
    e->sampleRateChanged(sr);
  }
}

void ElectrumVoice::renderNextSample(float& left,
                                     float& right,
                                     bool updateDests) {
  bool busy = isBusy();
  if (!busy) {
    if (wasBusy) {
      state->graph.voiceEnded(voiceIndex);
      wasBusy = false;
    }
    return;
  }
  // 1. tick the envelopes
  for (auto* e : envs)
    e->tick();
  vge.tick();
  // 2. update modulation dests if needed
  if (updateDests)
    _updateModDests(&state->modulations);
  // 3. add samples from the oscillators
  float voiceLeft = 0.0f;
  float voiceRight = 0.0f;
  for (int i = 0; i < NUM_OSCILLATORS; ++i) {
    oscs[i]->renderSampleStereo(currentNote, oscModState[i].levelMod,
                                oscModState[i].posMod, oscModState[i].panMod,
                                oscModState[i].coarseMod,
                                oscModState[i].fineMod, voiceLeft, voiceRight);
  }
  // 4. this is where filters and any other per-voice waveshaping happens
  // 5. add to the output
  left += voiceLeft * vge.getCurrentSample();
  right += voiceRight * vge.getCurrentSample();
  // 6. deal with any killQuick that may be happening
  if (inQuickKill && vge.getCurrentSample() == 0.0f) {
    inQuickKill = false;
    startNote(queuedNote, queuedVelocity);
  }
}

void ElectrumVoice::updateGraphData(GraphingData* gd) {
  // oscillators
  for (int i = 0; i < NUM_OSCILLATORS; ++i) {
    const float latestPos = std::clamp(
        audioState->wOsc[i].getPos() + oscModState[i].posMod, 0.0f, 1.0f);
    gd->updateOscPos(i, latestPos);
  }
  // envelopes
  for (int i = 0; i < NUM_ENVELOPES; ++i) {
    gd->updateEnvLevel(i, envs[i]->getCurrentSample());
  }
}
