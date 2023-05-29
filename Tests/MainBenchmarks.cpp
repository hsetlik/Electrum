#include <catch2/catch_test_macros.hpp>
#include "catch2/benchmark/catch_benchmark_all.hpp"
#include "PluginEditor.h"

//TEST_CASE ("Boot performance")
//{
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

TEST_CASE("Perlin generation")
{
    const size_t numSamples = 2000;
    BENCHMARK("octaves: 5, freq: 2, lac.: 2.5")
    {
        const size_t octaves = 5;
        const float freq = 2.0f;
        const float lac = 2.5f;
        std::vector<float> values(numSamples, 0.0f);
        PerlinGenerator gen;
        gen.setParams(octaves, freq, lac);
        for(size_t s = 0; s < numSamples; s++)
        {
            values[s] = gen.getNextValue();
        }
    };
    BENCHMARK("octaves: 1, freq: 2, lac.: 2.5")
    {
        const size_t octaves = 1;
        const float freq = 2.0f;
        const float lac = 2.5f;
        std::vector<float> values(numSamples, 0.0f);
        PerlinGenerator gen;
        gen.setParams(octaves, freq, lac);
        for(size_t s = 0; s < numSamples; s++)
        {
            values[s] = gen.getNextValue();
        }
    };
}

TEST_CASE("ElectrumAudioData")
{
    std::unique_ptr<ElectrumAudioData> ead = nullptr;
    BENCHMARK("Default constructor")
    {
        ead.reset(new ElectrumAudioData());
    };

    
}


