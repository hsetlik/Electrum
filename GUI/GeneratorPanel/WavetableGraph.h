#pragma once
#include "../../Parameters/Identifiers.h"
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"
#define WAVE_GRAPH_WIDTH 256
#define WAVE_GRAPH_HEIGHT 256
#define WAVE_REFRESH_HZ 24
#define WAVE_GRAPH_POINTS 128

class WavetableGraph : 
public Component,
public Timer
{
public:
    EVT* const state;
    const int index;
    WavetableGraph(EVT* tree, int idx);
    void paint(Graphics& g) override;
    void timerCallback() override;
private:
    Image img;
    // these static methods do the heavy lifting
    static std::vector<std::array<float, WAVE_GRAPH_POINTS>> getGraphDataFor(EVT* evt, int idx);
    static void renderWaveSetToImage(EVT* evt, Image& image, int idx);
    static void renderWaves(std::vector<std::array<float, WAVE_GRAPH_POINTS>>& waves, Image& img, float tablePos);
    static void renderSingleWave(std::array<float, WAVE_GRAPH_POINTS>& wave, Image& img, float tablePos, float currentPos);
    

};