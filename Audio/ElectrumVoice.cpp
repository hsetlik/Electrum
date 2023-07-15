#include "ElectrumVoice.h"

ElectrumVoice::ElectrumVoice (EVT* tree, ModDestMap* map, int idx) : 
state (tree),
modMap (map),
index (idx),
currentNote(-1),
currentNoteVelocity(0.0f),
gate(false),
filter(tree, idx)
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

bool ElectrumVoice::isBusy()
{
    for(auto e : envs)
    {
      if(!e->isFinished())
        return true;
    }
    return gate || (!env.isFinished());
}

void ElectrumVoice::startNote(int note, float vel)
{
    currentNote = note;
    currentNoteVelocity = vel;
    gate = true;
    env.gateStart();
    for(auto e : envs)
    {
        e->gateStart();
    }
    state->startVoice(index);
}

void ElectrumVoice::stopNote()
{
    env.gateEnd();
    gate = false;
    for(auto e : envs)
    {
        e->gateEnd();
    }
}

float ElectrumVoice::filterSample(float input)
{
  float currentCutoff = baseFilterCutoff + 
    ((float)Math::midiToHz(currentNote) * (baseFilterTracking + 
    getCurrentModDestValue(IDs::filterCutoff.toString()))) + 
    getCurrentModDestValue(IDs::filterCutoff.toString());

  float currentRes = baseFilterRes + getCurrentModDestValue(IDs::filterResonance.toString());
  float currentMix = baseFilterMix + getCurrentModDestValue(IDs::filterMix.toString());
  
  float filtered = filter.process(input, baseFilterType, currentCutoff, currentRes);
  return Math::flerp(input, filtered, currentMix);
}

void ElectrumVoice::renderNextSample(float& left, float& right)
{
    if (!isBusy())
        return;
    //tick the modulation sources before we calculate any mod values
    for(auto e : envs)
        e->tick();
    float output = 0.0f;
    for (auto o : oscs)
    {
        float levelMod = getCurrentModDestValue(o->getLevelParamName());
        float posMod = getCurrentModDestValue(o->getPosParamName());
        jassert(posMod < 1.0f);
        output += o->getNextSample(Math::midiToHz(currentNote), AudioSystem::getSampleRate(), posMod, levelMod);
    }
    output = output * env.getSample() * 0.25f;
    output = filterSample(output);
    left += output;
    right += output;
}

void ElectrumVoice::updateForBlock()
{
    for (auto o : oscs)
    {
        o->updateBasePos();
        o->updateBaseLevel();
    }
    baseFilterCutoff = state->getFloatParamValue(IDs::filterCutoff.toString());
    baseFilterRes = state->getFloatParamValue(IDs::filterResonance.toString());
    baseFilterMix = state->getFloatParamValue(IDs::filterMix.toString());
    baseFilterTracking = state->getFloatParamValue(IDs::filterTracking.toString());
    baseFilterType = state->getCurrentFilterType();
    // if this is currently the newest voice, update levels for the graphics side
    if(state->currentNewestVoice() == index && state->isEditorOpen())
    {
     for(int i = 0; i < NUM_ENVELOPES; i++)
     {
      state->setLeadingVoiceEnvLevel(i, envs[i]->getCurrentSample());
     }
    }
}


float ElectrumVoice::getModValueForSample(const String& srcID)
{
    auto safeID = StringUtil::removeTrailingNumbers(srcID);
    if (srcID == IDs::modWheelSource.toString())
    {
        return state->getModWheel();
    }
    else if (srcID == IDs::pitchWheelSource.toString())
    {
        return state->getPitchBend();
    }
    else if(srcID == IDs::perlinSource.toString())
    {
        return state->perlinValue();
    }
    else if(safeID == IDs::envSource.toString())
    {
        String numStr = srcID.trimCharactersAtStart(safeID);
        int idx = std::stoi(numStr.toStdString());
        return envs[idx]->getCurrentSample();
    }
    else
    {
        return 0.0f;
    }
}

float ElectrumVoice::getCurrentModDestValue(const String& destID)
{
    auto destIt = modMap->find(destID);
    if (destIt == modMap->end())
        return 0.0f;
    auto& destMap = destIt->second;
    float value = 0.0f;
    for(auto it = destMap.begin(); it != destMap.end(); ++it)
    {
        value += getModValueForSample(it->first) * it->second;
    }
    return value;
}
