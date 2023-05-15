#include "ElectrumAudioData.h"

ElectrumAudioData::ElectrumAudioData()
{
    std::cout << "Initializing Electrum Audio Data. . .\n";
    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
        std::cout << "Adding wavetabe set number: " << i << "...\n";
        oscillators.add(new WavetableSet(WaveUtil::getDefaultWaveSet()));
        auto oscString = oscillators.getLast()->getWavesAsString();
        std::cout << oscillators.getLast()->numWaves() << " waves stored in " << oscString.size() << " bytes\n";
    }

}

ElectrumAudioData::ElectrumAudioData(ValueTree& state)
{
    //TODO
}

float ElectrumAudioData::getOscillatorValue(int idx, float phase, float tablePos, double freq, double sampleRate)
{
    jassert(idx < oscillators.size());
    return oscillators[idx]->getSample(phase, tablePos, freq, sampleRate);
}
