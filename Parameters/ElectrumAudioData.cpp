#include "ElectrumAudioData.h"
#include "Identifiers.h"

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
  for (int i = 0; i < NUM_LFOS; i++)
  {
    lfoData.add(new LFOData());
  }
}

ElectrumAudioData::ElectrumAudioData(ValueTree &state)
{
  // add the envelopes and the LFOs as normal bc those don't get stored separately
  for (int i = 0; i < NUM_ENVELOPES; i++)
  {
    envData.add(new AHDSRData());
  }
  for (int i = 0; i < NUM_LFOS; i++)
  {
    lfoData.add(new LFOData());
  }
  // find any children with type WAVETABLE_DATA
  if (!state.hasType(IDs::ELECTRUM_AUDIO_DATA))
    DLog::log("Not a valid ElectrumAudioData tree!");
  // loop thru the children
  for (auto it = state.begin(); it != state.end(); ++it)
  {
    auto child = *it;
    if (child.hasType(IDs::WAVETABLE_DATA))
    {
      String data = child[IDs::wavetableStringData];
      oscillators.add(new WavetableSet(data));
    }
  }
}

ValueTree ElectrumAudioData::toValueTree()
{
  ValueTree tree(IDs::ELECTRUM_AUDIO_DATA);
  for (int i = 0; i < NUM_OSCILLATORS; i++)
  {
    tree.appendChild(oscillators[i]->toValueTree(), nullptr);
  }
  return tree;
}

float ElectrumAudioData::getOscillatorValue(int idx, float phase, float tablePos, double freq,
                                            double sampleRate)
{
  return oscillators[idx]->getSample(phase, tablePos, freq, sampleRate);
}
