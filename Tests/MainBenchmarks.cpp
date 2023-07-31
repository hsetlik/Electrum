#include "TestUtil.h"
#if BUILD_BENCHMARKS
//TEST_CASE ("Boot performance")

//     BENCHMARK_ADVANCED ("Processor constructor")
//     (Catch::Benchmark::Chronometer meter)
//     {
//         auto gui = juce::ScopedJuceInitialiser_GUI {};
//         std::vector<Catch::Benchmark::storage_for<ElectrumAudioProcessor>> storage (size_t (meter.runs()));
//         meter.measure ([&] (int i) { storage[(size_t) i].construct(); });
//     };

//     BENCHMARK_ADVANCED ("Processor destructor")
//     (Catch::Benchmark::Chronometer meter)
//     {
//         auto gui = juce::ScopedJuceInitialiser_GUI {};
//         std::vector<Catch::Benchmark::destructable_object<ElectrumAudioProcessor>> storage (size_t (meter.runs()));
//         for (auto& s : storage)
//             s.construct();
//         meter.measure ([&] (int i) { storage[(size_t) i].destruct(); });
//     };

//     BENCHMARK_ADVANCED ("Editor open and close")
//     (Catch::Benchmark::Chronometer meter)
//     {
//         auto gui = juce::ScopedJuceInitialiser_GUI {};

//         ElectrumAudioProcessor plugin;

//         // due to complex construction logic of the editor, let's measure open/close together
//         meter.measure ([&] (int i) {
//             auto editor = plugin.createEditorIfNeeded();
//             plugin.editorBeingDeleted (editor);
//             delete editor;
//             return plugin.getActiveEditor();
//         });
//     };
//}

TEST_CASE("WavetableSet Construction")
{
    BENCHMARK_ADVANCED("10 square waves")
    (Catch::Benchmark::Chronometer meter)
    {
        auto gui= juce::ScopedJuceInitialiser_GUI {};

        std::vector<Wave> waves = 
        {
            WaveUtil::getPulseWave(0.1f),
            WaveUtil::getPulseWave(0.2f),
            WaveUtil::getPulseWave(0.3f),
            WaveUtil::getPulseWave(0.4f),
            WaveUtil::getPulseWave(0.5f),
            WaveUtil::getPulseWave(0.6f),
            WaveUtil::getPulseWave(0.7f),
            WaveUtil::getPulseWave(0.8f),
            WaveUtil::getPulseWave(0.9f),
            WaveUtil::getPulseWave(0.99f)
        };
        std::vector<Catch::Benchmark::storage_for<WavetableSet>> storage (size_t (meter.runs()));
        meter.measure ([&] (int i) { storage[(size_t) i].construct<WavetableSet>(waves); });
    };
}

// TEST_CASE("Perlin generation")
// {
//     const size_t numSamples = 2000;
//     BENCHMARK("octaves: 5, freq: 2, lac.: 2.5")
//     {
//         const size_t octaves = 5;
//         const float freq = 2.0f;
//         const float lac = 2.5f;
//         std::vector<float> values(numSamples, 0.0f);
//         PerlinGenerator gen;
//         gen.setParams(octaves, freq, lac);
//         for(size_t s = 0; s < numSamples; s++)
//         {
//             values[s] = gen.getNextValue();
//         }
//     };
//     BENCHMARK("octaves: 1, freq: 2, lac.: 2.5")
//     {
//         const size_t octaves = 1;
//         const float freq = 2.0f;
//         const float lac = 2.5f;
//         std::vector<float> values(numSamples, 0.0f);
//         PerlinGenerator gen;
//         gen.setParams(octaves, freq, lac);
//         for(size_t s = 0; s < numSamples; s++)
//         {
//             values[s] = gen.getNextValue();
//         }
//     };
// }

TEST_CASE("ElectrumEngine tests")
{
    auto gui = juce::ScopedJuceInitialiser_GUI {};
    const int length1 = 2000; 
    std::unique_ptr<ElectrumAudioProcessor> proc = std::make_unique<ElectrumAudioProcessor>();
    AudioBuffer<float> testBuffer1(2, length1);
    auto midiBuffer = TestUtil::getChordBuffer(length1, 2, 0.8f);
    proc->prepareToPlay(44100.0f, length1);
    BENCHMARK("2000 samples, 2 note chord")
    {
        proc->processBlock(testBuffer1, midiBuffer);
    };
    const int length2 = 2000;
    proc.reset(new ElectrumAudioProcessor());
    auto midiBuffer2 =  TestUtil::getChordBuffer(length2, 4, 0.8f);

    AudioBuffer<float> testBuffer2(2, length2);
    proc->prepareToPlay(44100.0f, length2);
    BENCHMARK("2000 samples, 4 note chord")
    {
        proc->processBlock(testBuffer2, midiBuffer2);
    };
    proc.reset(new ElectrumAudioProcessor());
    proc->prepareToPlay(44100.0f, length2);
    auto midiBuffer3 =  TestUtil::getChordBuffer(length2, 8, 0.8f);
    testBuffer2.clear();
    BENCHMARK("2000 samples, 8 note chord")
    {
        proc->processBlock(testBuffer2, midiBuffer3);
    };
}

TEST_CASE("Oscillator benchmarks")
{
    auto gui = juce::ScopedJuceInitialiser_GUI {};
    std::unique_ptr<ElectrumAudioProcessor> proc = std::make_unique<ElectrumAudioProcessor>();   
    auto& state = proc->state;
    BENCHMARK("getOscillatorValue x 5000")
    {
        const int numSamples = 5000;
        const float phaseDelta = 440.0f / 44100.0f;
        const float pDelta = 1.0f / (float)numSamples;
        std::vector<float> outputValues;
        for(int s = 0; s < numSamples; s++)
        {
            outputValues.push_back(state.getOscillatorValue(0, phaseDelta * (float)s, pDelta * (float)s, 440.0f, 44100.0f));
        }
    };
}

TEST_CASE("flerp Benchmarks")
{
    auto& rand = Random::getSystemRandom();
    std::vector<float> aVals;
    std::vector<float> bVals;
    std::vector<float> tVals;
    const int numLerps = 10;
    for(int i = 0; i < numLerps; i++)
    {
       aVals.push_back(rand.nextFloat());
       bVals.push_back(rand.nextFloat());
       tVals.push_back(rand.nextFloat());
    }

    BENCHMARK("flerp")
    {
        std::vector<float> output;
        for(int i = 0; i < numLerps; i++)
        {
            output.push_back(Math::flerp(aVals[(size_t)i], bVals[(size_t)i], tVals[(size_t)i]));
        }
    };

    std::vector<float> currentVals;
    tVals.clear();
    for(int i = 0; i < numLerps; i++)
    {
      tVals.push_back((rand.nextFloat() * 2.0f) - 1.0f);
      currentVals.push_back(rand.nextFloat() * 1000.0f);
    }

    BENCHMARK("bipolarFlerp")
    {
      std::vector<float> output;
      for(size_t i = 0; i < numLerps; i++)
      {
       output.push_back(Math::bipolarFlerp(aVals[i], bVals[i], currentVals[i], tVals[i])); 
      }
    };

}

TEST_CASE("easing curve benchmarks")
{
    BENCHMARK("Curve: 100")
    {
        std::vector<float> output;
        for(int i = 0; i < 100; i++)
        {
            float x = (float)i / 100.0f;
            output.push_back(Math::onEasingCurve(0.0f, x, 1.0f, x));
        }
    };
}

#endif //BUILD_BENCHMARKS
