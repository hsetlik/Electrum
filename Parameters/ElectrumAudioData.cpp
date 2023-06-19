#include "ElectrumAudioData.h"

ElectrumAudioData::ElectrumAudioData()
{
    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
        oscillators.add(new WavetableSet(WaveUtil::getDefaultWaveSet()));
    }
    for (int i = 0; i < NUM_ENVELOPES; i++)
    {
        envData.add(new AHDSRData());
    }

}

ElectrumAudioData::ElectrumAudioData(ValueTree& state)
{
    //find any children with type WAVETABLE_DATA
}

float ElectrumAudioData::getOscillatorValue(int idx, float phase, float tablePos, double freq, double sampleRate)
{
    return oscillators[idx]->getSample(phase, tablePos, freq, sampleRate);
}
