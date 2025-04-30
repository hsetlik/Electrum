#include "Electrum/Audio/Synth/Voice.h"
#include "Electrum/Audio/Modulator/AHDSR.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_audio_basics/juce_audio_basics.h"
//===================================================

static const float minEnvelopeLvl = juce::Decibels::decibelsToGain(-24.0f);

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

bool VoiceGateEnvelope::isFinished() const {
  return !gate && lastOutput <= minEnvelopeLvl;
}

//===================================================

float* FilterSumHandler::filterLeft(int f) {
  size_t i = (size_t)(2 * f);
  return &data[i];
}

float* FilterSumHandler::filterRight(int f) {
  size_t i = (size_t)(2 * f) + 1;
  return &data[i];
}
// call this with the references to the passed in samples
void FilterSumHandler::addCurrentSumTo(float& left, float& right) const {
  for (size_t i = 0; i < 3; ++i) {
    left += data[(2 * i)];
    right += data[(2 * i) + 1];
  }
}

//
// the input value coresponds to the 'mod_source_t' enum
// in ElectrumState.h
float ElectrumVoice::_currentModSrcVal(int src) {
  const ModSourceE id = (ModSourceE)src;
  if (id < ModSourceE::LFO1) {
    return envs[src]->getCurrentSample();
  } else if (id < ModSourceE::ModWheel) {
    return lfos[src - NUM_ENVELOPES]->getCurrentSample();
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
  int filterIdx = 0;
  while (oscIdx < NUM_OSCILLATORS && destIdx < (int)ModDestE::filt1Cutoff) {
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
  while (destIdx < MOD_DESTS && filterIdx < NUM_FILTERS) {
    filters[filterIdx]->setCutoffMod(
        _normalizedModulationForDest(map, destIdx));
    ++destIdx;
    filters[filterIdx]->setResonanceMod(
        _normalizedModulationForDest(map, destIdx));
    ++destIdx;
    filters[filterIdx]->setGainMod(_normalizedModulationForDest(map, destIdx));
    ++destIdx;
    ++filterIdx;
  }
}

//===================================================
ElectrumVoice::ElectrumVoice(ElectrumState* s, int idx)
    : state(s), vge(this), voiceIndex(idx) {
  // instantiate the oscillators
  for (int i = 0; i < NUM_OSCILLATORS; i++) {
    oscs.add(new WavetableOscillator(&state->audioData.wOsc[i], i));
  }
  // instantiate the envelopes
  for (int i = 0; i < NUM_ENVELOPES; ++i) {
    envs.add(new AHDSREnvelope(&state->audioData.env[i], i));
  }
  // instantiate the LFOs
  for (int i = 0; i < NUM_LFOS; i++) {
    lfos.add(new VoiceLFO(&state->audioData.lfos[i]));
  }
  // instantiate the filters
  for (int i = 0; i < NUM_FILTERS; ++i) {
    auto* params = &state->audioData.filters[i];
    filters.add(new VoiceFilter(params));
  }
}

bool ElectrumVoice::isBusy() const {
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
  for (auto* l : lfos) {
    l->gateStarted();
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
  gate = false;
  vge.end();
  for (auto e : envs) {
    e->gateEnd();
  }
}

void ElectrumVoice::sampleRateSet(double sr) {
  for (auto* e : envs) {
    e->sampleRateChanged(sr);
  }
  for (auto* f : filters) {
    f->prepare(sr);
  }
}

void ElectrumVoice::addToFilterSums(float oscL, float oscR, int oscID) {
  bool filtered = false;
  auto& params1 = state->audioData.filters[0];
  if (params1.oscActive[(size_t)oscID]) {
    filtered = true;
    filterSums.addToFilter1(oscL, oscR);
  }
  auto& params2 = state->audioData.filters[1];
  if (params2.oscActive[(size_t)oscID]) {
    filtered = true;
    filterSums.addToFilter2(oscL, oscR);
  }
  if (!filtered) {
    filterSums.addToDry(oscL, oscR);
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
  // 1. tick the envelopes and LFOs
  for (auto* e : envs)
    e->tick();
  for (auto* l : lfos)
    l->tick();
  vge.tick();
  // 2. update modulation dests if needed
  if (updateDests)
    _updateModDests(&state->modulations);
  // 3. add samples from the oscillators
  filterSums.clear();
  for (int i = 0; i < NUM_OSCILLATORS; ++i) {
    float oscLeft = 0.0f;
    float oscRight = 0.0f;
    oscs[i]->renderSampleStereo(currentNote, oscModState[i].levelMod,
                                oscModState[i].posMod, oscModState[i].panMod,
                                oscModState[i].coarseMod,
                                oscModState[i].fineMod, oscLeft, oscRight);
    addToFilterSums(oscLeft, oscRight, i);
  }
  // 4. this is where filters and any other per-voice waveshaping happens
  for (int i = 0; i < NUM_FILTERS; ++i) {
    filters[i]->processStereo(*filterSums.filterLeft(i),
                              *filterSums.filterRight(i));
  }
  // 5. add to the output
  const float gateLvl = vge.getCurrentSample();
  auto vLeft = filterSums.getLeftSum() * gateLvl;
  auto vRight = filterSums.getRightSum() * gateLvl;
  rms.tick(vLeft, vRight);
  left += vLeft;
  right += vRight;
  // 6. deal with any killQuick that may be happening
  if (inQuickKill && gateLvl <= minEnvelopeLvl) {
    inQuickKill = false;
    startNote(queuedNote, queuedVelocity);
  }
}

void ElectrumVoice::updateForBlock() {
  for (int i = 0; i < NUM_FILTERS; ++i) {
    filters[i]->updateForBlock();
  }
}

void ElectrumVoice::updateGraphData(GraphingData* gd) {
  // oscillators
  for (int i = 0; i < NUM_OSCILLATORS; ++i) {
    const float latestPos = std::clamp(
        state->audioData.wOsc[i].getPos() + oscModState[i].posMod, 0.0f, 1.0f);
    gd->updateOscPos(i, latestPos);
  }
  // envelopes
  for (int i = 0; i < NUM_ENVELOPES; ++i) {
    gd->updateEnvLevel(i, envs[i]->getCurrentSample());
  }
  // LFOs
  for (int i = 0; i < NUM_LFOS; ++i) {
    gd->updateLFOPhase(i, lfos[i]->getCurrentPhase());
  }
}
