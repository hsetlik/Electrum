#include "ElectrumAudioData.h"

ElectrumAudioData::ElectrumAudioData()
{
    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
        oscillators.add(new WavetableSet(WaveUtil::getDefaultWaveSet()));
    }

}

ElectrumAudioData::ElectrumAudioData(ValueTree& state)
{
    //find any children with type WAVETABLE_DATA
}

float ElectrumAudioData::getOscillatorValue(int idx, float phase, float tablePos, double freq, double sampleRate)
{
    jassert(idx < oscillators.size());
    return oscillators[idx]->getSample(phase, tablePos, freq, sampleRate);
}
