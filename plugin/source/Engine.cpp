#include "Electrum/Audio/Synth/Engine.h"
#include "juce_core/juce_core.h"

ElectrumVoice* SynthEngine::getFreeVoice() {
  for (auto* v : voices) {
    if (!v->isBusy())
      return v;
  }
  return nullptr;
}

ElectrumVoice* SynthEngine::getVoicePlayingNote(int note) {
  for (auto v : voices) {
    if (v->getCurrentNote() == note && v->isBusy())
      return v;
  }
  return nullptr;
}

void SynthEngine::noteOn(int note, float velocity) {
  auto existing = getVoicePlayingNote(note);
  if (existing != nullptr) {
    existing->stealNote(note, velocity);
  } else {
    auto voice = getFreeVoice();
    jassert(voice != nullptr);
    voice->startNote(note, velocity);
  }
}

void SynthEngine::noteOff(int note) {
  auto voice = getVoicePlayingNote(note);
  if (voice != nullptr && !state->getSustainPedal()) {
    voice->stopNote();
  } else if (voice != nullptr)  // handle the sustain pedal logic here
  {
    sustainedVoices.push(voice);
  }
}

void SynthEngine::killSustainedVoices() {
  while (!sustainedVoices.empty()) {
    auto* v = sustainedVoices.front();
    sustainedVoices.pop();
    v->stopNote();
  }
}

// MIDI handling stuff -----------------------------

void SynthEngine::loadMidiEvents(juce::MidiBuffer& midi) {
  for (auto it = midi.begin(); it != midi.end(); ++it) {
    auto metadata = *it;
    timed_midi_msg m;
    m.timestamp = metadata.samplePosition;
    m.message = metadata.getMessage();
    midiQueue.push(m);
  }
}

void SynthEngine::handleMidiMessage(juce::MidiMessage& message) {
  // big ol else if to handle every type of MIDI message
  if (message.isNoteOn()) {
    noteOn(message.getNoteNumber(), message.getFloatVelocity());
  } else if (message.isNoteOff()) {
    noteOff(message.getNoteNumber());
  } else if (message.isSustainPedalOn()) {
    state->setSustainPedal(true);
  } else if (message.isSustainPedalOff()) {
    state->setSustainPedal(false);
    killSustainedVoices();
  } else if (message.isController() &&
             message.getControllerNumber() == 1)  // handle mod wheel
  {
    float modVal = (float)message.getControllerValue() / 127.0f;
    state->setModWheel(modVal);
  } else if (message.isPitchWheel()) {
    // state->setPitchBend(Math::toPitchBendValue(message.getPitchWheelValue()));
  } else {
    // DLog::log("Warning! Unhandled MIDI message: " +
    // message.getDescription());
  }
}

//===================================================

SynthEngine::SynthEngine(ElectrumState* s) : state(s) {
  for (int i = 0; i < NUM_VOICES; ++i) {
    voices.add(new ElectrumVoice(s, &audioSrc, i));
  }
}

void SynthEngine::processBlock(juce::AudioBuffer<float>& audioBuf,
                               juce::MidiBuffer& midiBuf) {
  juce::ignoreUnused(audioBuf, midiBuf);
}

void SynthEngine::prepareToPlay(double sampleRate, size_t blockSize) {
  juce::ignoreUnused(sampleRate, blockSize);
}
