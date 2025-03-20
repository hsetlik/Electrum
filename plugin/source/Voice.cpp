#include "Electrum/Audio/Synth/Voice.h"

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
  // TODO: check if any of the parent's
  //  envelopes are still running
  //  for (auto e : parent->envs)
  //  {
  //    if (!e->isFinished())
  //      return false;
  //  }
  return true;
}

//===================================================
ElectrumVoice::ElectrumVoice(ElectrumState* s, AudioSourceState* a, int idx)
    : state(s), audioState(a), vge(this), voiceIndex(idx) {
  for (int i = 0; i < NUM_OSCILLATORS; i++) {
    wOscs.add(new WavetableOscillator(&audioState->wOsc[i], i));
  }
}

bool ElectrumVoice::isBusy() {
  return gate || (!vge.isFinished());
}

void ElectrumVoice::startNote(int note, float vel) {
  currentNote = note;
  currentNoteVelocity = vel;
  gate = true;
  // DLog::log("New note at voice: " + String(index));
  vge.start();
  // TODO: start the mod envelopes here
}

