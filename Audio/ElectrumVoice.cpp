#include "ElectrumVoice.h"

ElectrumVoice::ElectrumVoice (EVT* tree, ModDestMap* map, int idx) : 
state (tree),
modMap (map),
index (idx),
currentNote(-1),
currentNoteVelocity(0.0f),
gate(false)
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

void ElectrumVoice::renderNextSample(float& left, float& right)
{
    if (!isBusy())
        return;
    static float output = 0.0f;
    output = 0.0f;
    for (auto o : oscs)
    {
        static float levelMod = 0.0f;
        static float posMod = 0.0f;
        levelMod = getCurrentModDestValue(o->getLevelParamName());
        posMod = getCurrentModDestValue(o->getPosParamName());
        jassert(posMod < 1.0f);
        output += o->getNextSample(Math::midiToHz(currentNote), AudioSystem::getSampleRate(), posMod, levelMod);
    }
    output = output * env.getSample() * 0.25f;
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
        return envs[idx]->getNextSample();
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