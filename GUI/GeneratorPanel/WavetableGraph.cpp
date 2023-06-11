#include "WavetableGraph.h"

WavetableGraph::WavtableGraph(EVT* tree, int idx) :
state(tree),
index(idx),
img(WAVE_GRAPH_WIDTH, WAVE_GRAPH_HEIGHT, true)
{
    startTimerHz(WAVE_REFRESH_HZ);
}

void WavetableGraph::timerCallback()
{

}

void WavetableGraph::paint(Graphics& g)
{

}

//==================================================================================================
std::vector<std::array<float, WAVE_GRAPH_POINTS>> WavetableGraph::getGraphDataFor(EVT* evt, int idx)
{
    std::vector<std::array<float, WAVE_GRAPH_POINTS>> output;
    auto waves = evt->getAudioData()->getBaseWaves(idx);
    for(auto wave : waves)
    {
        std::array<float, WAVE_GRAPH_POINTS> arr;
        for(int i = 0; i < WAVE_GRAPH_POINTS; i++)
        {
            float phase = (float)i / (float)WAVE_GRAPH_POINTS;
            arr[i] = WaveUtil::valueAtPhase(wave, phase);
        }
        output.push_back(arr);
    }
    return output;
}

void WavetableGraph::renderWaveToImage(EVT* evt, Image& image, int idx)
{
    auto waves = evt->getAudioData()->getBaseWaves(idx);
    float tablePos = evt->getFloatParamValue(IDs::oscillatorPos.toString() + String(idx));
    renderWave(waves, image, tablePos);
}

void WavetableGraph::renderWave(std::vector<std::array<float, WAVE_GRAPH_POINTS>>& waves, Image& img, float tablePos)
{

}