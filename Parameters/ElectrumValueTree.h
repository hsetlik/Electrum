#pragma once
#include "../Core/CustomJuceHeader.h"
#include "Identifiers.h"

class EVT : public AudioProcessorValueTreeState
{
private:
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        AudioProcessorValueTreeState::ParameterLayout layout;

        return layout;
    }
public:
    EVT(AudioProcessor &proc,
         UndoManager *undo,
         const Identifier &valueTreeType) : 
         AudioProcessorValueTreeState(proc, undo, valueTreeType, createParameterLayout())
    {

    }
};