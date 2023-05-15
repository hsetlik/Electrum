#pragma once
#include "../Core/CustomJuceHeader.h"
#include "ElectrumAudioData.h"

class EVT : public AudioProcessorValueTreeState
{
private:
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        AudioProcessorValueTreeState::ParameterLayout layout;

        return layout;
    }
    std::unique_ptr<ElectrumAudioData> audioData;
public:
    EVT(AudioProcessor &proc,
         UndoManager *undo,
         const Identifier &valueTreeType) : 
         AudioProcessorValueTreeState(proc, undo, valueTreeType, createParameterLayout()),
         audioData(std::make_unique<ElectrumAudioData>())
    {

    }

    float getOscillatorValue(int idx, float phase, float tablePos, double freq, double sampleRate)
    {
        return audioData->getOscillatorValue(idx, phase, tablePos, freq, sampleRate);
    }

};