#include "Electrum/Audio/Synth/Engine.h"
#include "Electrum/Identifiers.h"
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
    state->graph.voiceStarted(voice->voiceIndex);
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

// GUI/DSP communication stuff---------------------
void SynthEngine::updateParamsForBlock() {
  state->updateCommonAudioData();
}

// MIDI handling stuff -----------------------------

void SynthEngine::loadMidiEvents(juce::MidiBuffer& midi, int maxSample) {
  for (auto it = midi.begin(); it != midi.end(); ++it) {
    auto metadata = *it;
    timed_midi_msg m;
    m.timestamp = metadata.samplePosition;
    jassert(m.timestamp >= 0 && m.timestamp < maxSample);
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
    DLog::log("Warning! Unhandled MIDI message: " + message.getDescription());
  }
}

void SynthEngine::renderNextSample(float& left,
                                   float& right,
                                   bool updateDests) {
  for (auto* v : voices) {
    v->renderNextSample(left, right, updateDests);
  }
}

//===================================================

SynthEngine::SynthEngine(ElectrumState* s) : state(s) {
  for (int i = 0; i < NUM_VOICES; ++i) {
    voices.add(new ElectrumVoice(s, i));
  }
}

void SynthEngine::processBlock(juce::AudioBuffer<float>& audioBuf,
                               juce::MidiBuffer& midiBuf) {
  // 1. grab any needed updates from the GUI
  updateParamsForBlock();
  // 1b. check if the GUI wants graphing data updates
  if (state->graph.wantsUpdate()) {
    auto* v = voices[state->graph.getNewestVoiceIndex()];
    jassert(v != nullptr);
    v->updateGraphData(&state->graph);
    // update the wavetable strings if necessary
    if (state->graph.needsWavetableData()) {
      for (int i = 0; i < NUM_OSCILLATORS; ++i) {
        auto str = state->audioData.wOsc[i].toString();
        state->graph.updateWavetableString(str, i);
      }
      DLog::log("Wavetable strings updated");
    }
    state->graph.updateFinished();
  }

  // 2. load midi events into the queue (and load any events from the GUI
  // keyboard)
  masterKeyboardState.processNextMidiBuffer(midiBuf, 0,
                                            audioBuf.getNumSamples(), true);
  loadMidiEvents(midiBuf, audioBuf.getNumSamples());
  // 3. determine if we're stereo or mono
  if (audioBuf.getNumChannels() >= 2) {
    // 4. render the audio
    float* lSample = audioBuf.getWritePointer(0);
    float* rSample = audioBuf.getWritePointer(1);
    for (int i = 0; i < audioBuf.getNumSamples(); ++i) {
      // process any midi events for this sample
      while (!midiQueue.empty() && midiQueue.front().timestamp <= i) {
        handleMidiMessage(midiQueue.front().message);
        midiQueue.pop();
      }
      renderNextSample(lSample[i], rSample[i], destUpdateIdx == 0);
      destUpdateIdx = (destUpdateIdx + 1) % DEST_UPDATE_INTERVAL;
    }
  } else {
    float* lSample = audioBuf.getWritePointer(0);
    float dummy;
    for (int i = 0; i < audioBuf.getNumSamples(); ++i) {
      // process any midi events for this sample
      while (!midiQueue.empty() && midiQueue.front().timestamp == i) {
        handleMidiMessage(midiQueue.front().message);
        midiQueue.pop();
      }
      renderNextSample(lSample[i], dummy, destUpdateIdx == 0);
      destUpdateIdx = (destUpdateIdx + 1) % DEST_UPDATE_INTERVAL;
    }
  }
}

void SynthEngine::prepareToPlay(double sampleRate, int blockSize) {
  for (auto* v : voices) {
    v->sampleRateSet(sampleRate);
  }
  juce::ignoreUnused(blockSize);
}
