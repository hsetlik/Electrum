#include "ElectrumEngine.h"

ElectrumEngine::ElectrumEngine(EVT *tree)
    : destUpdateIdx(DEST_UPDATE_INTERVAL), // set this high so we update
                                           // everything on the first sample
      state(tree)
{
  for (int i = 0; i < NUM_VOICES; i++)
  {
    voices.add(new ElectrumVoice(state, &currentModulation, i));
  }
}

void ElectrumEngine::noteOn(int note, float velocity)
{
  auto existing = getVoicePlayingNote(note);
  if (existing != nullptr)
  {
    existing->stealNote(note, velocity);
  } else
  {
    auto voice = getFreeVoice();
    jassert(voice != nullptr);
    voice->startNote(note, velocity);
  }
}

void ElectrumEngine::noteOff(int note)
{

  auto voice = getVoicePlayingNote(note);
  if (voice != nullptr)
  {
    voice->stopNote();
  } else
  {
    DLog::log("No voice found for note: " + String(note));
  }
}

ElectrumVoice *ElectrumEngine::getFreeVoice()
{
  for (auto v : voices)
  {
    if (!v->isBusy())
      return v;
  }
  DLog::log("Warning! No free voices!");
  return nullptr;
}

ElectrumVoice *ElectrumEngine::getVoicePlayingNote(int note)
{
  for (auto v : voices)
  {
    if (v->getCurrentNote() == note && v->isBusy())
      return v;
  }
  return nullptr;
}

void ElectrumEngine::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midi)
{
  //    TRACE_DSP();
  updateParamsForBlock();
  state->loadModulationData(currentModulation);
  // this loads the midi events and their timestamps into a queue
  loadMidiEvents(midi);
  // make sure we have stereo
  jassert(buffer.getNumChannels() >= 2);
  for (int s = 0; s < buffer.getNumSamples(); s++)
  {
    // STEP 1: Check if we have a midi event on this sample
    //
    // there's a good reason this is a while rather than a for loop: many GUI
    // plugin hosts will send multiple midi events with the same timestamp
    while (!midiQueue.empty() && midiQueue.front().timestamp == s)
    {
      handleMidiMessage(midiQueue.front().message);
      midiQueue.pop();
    }
    float left = 0.0f;
    float right = 0.0f;
    // STEP 2: figure out whether to update the mod dests or not
    if (destUpdateIdx >= DEST_UPDATE_INTERVAL)
    {
      destUpdateIdx = 0;
      // STEP 3: Render the actual samples
      renderNextSample(left, right, true);
    } else
    {
      ++destUpdateIdx;
      renderNextSample(left, right, false);
    }
    buffer.setSample(0, s, left);
    buffer.setSample(1, s, right);
  }
  // ensure that the midi queue is empty (i.e. no out of range timestamps)
  // jassert(midiQueue.empty());
}

void ElectrumEngine::renderNextSample(float &l, float &r, bool updateDests)
{
  state->tickPerlinForSample();
  for (auto v : voices)
  {
    v->renderNextSample(l, r, updateDests);
  }
}

int ElectrumEngine::numBusyVoices()
{
  int count = 0;
  for (auto v : voices)
  {
    if (v->isBusy())
      ++count;
  }
  return count;
}

// note: this just updates the base value of the various parameters, modulation
// is dealt with elsewhere
void ElectrumEngine::updateParamsForBlock()
{
  state->updatePerlinForBlock();
  state->updateEnvelopesForBlock();
  // check to update state for voices that have trailed off
  for (int i = 0; i < NUM_VOICES; i++)
  {
    voices[i]->updateForBlock();
    if (state->isVoiceActive(i) && !voices[i]->isBusy())
      state->endVoice(i);
  }
}

void ElectrumEngine::loadMidiEvents(MidiBuffer &midi)
{
  for (auto it = midi.begin(); it != midi.end(); ++it)
  {
    auto metadata = *it;
    TimestampedMidiMessage m;
    m.timestamp = metadata.samplePosition;
    m.message = metadata.getMessage();
    midiQueue.push(m);
  }
}

void ElectrumEngine::handleMidiMessage(MidiMessage &message)
{
  // big ol else if to handle every type of MIDI message
  if (message.isNoteOn())
  {
    noteOn(message.getNoteNumber(), message.getFloatVelocity());
  } else if (message.isNoteOff())
  {
    noteOff(message.getNoteNumber());
  } else if (message.isSustainPedalOn())
  {
    state->setSustainPedal(true);
  } else if (message.isSustainPedalOff())
  {
    state->setSustainPedal(false);
  } else if (message.isController() && message.getControllerNumber() == 1) // handle mod wheel
  {
    float modVal = (float)message.getControllerValue() / 127.0f;
    state->setModWheel(modVal);
  } else if (message.isPitchWheel())
  {
    state->setPitchBend(Math::toPitchBendValue(message.getPitchWheelValue()));
  } else
  {
    DLog::log("Warning! Unhandled MIDI message: " + message.getDescription());
  }
}
//=============================================================
