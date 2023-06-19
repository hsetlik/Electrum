#include "WavetableGraph.h"

WavetableGraph::WavetableGraph(EVT* tree, int idx) :
state(tree),
index(idx),
img(Image::PixelFormat::RGB, WAVE_GRAPH_WIDTH, WAVE_GRAPH_HEIGHT, true)
{
    startTimerHz(WAVE_REFRESH_HZ);
}

void WavetableGraph::timerCallback()
{
    renderWaveSetToImage(state, img, index);
    repaint();
}

void WavetableGraph::paint(Graphics& g)
{
   g.drawImage(img, getLocalBounds().toFloat()); 
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

void WavetableGraph::renderWaveSetToImage(EVT* evt, Image& image, int idx)
{
    auto waves = evt->getAudioData()->getBaseWaves(idx);
    std::vector<std::array<float, WAVE_GRAPH_POINTS>> graphWaves;
    for(auto wave : waves)
    {
        std::array<float, WAVE_GRAPH_POINTS> arr;
        for(int i = 0; i < WAVE_GRAPH_POINTS; ++i)
        {
            float phase = (float)i / (float)WAVE_GRAPH_POINTS;
            arr[(size_t)i] = WaveUtil::valueAtPhase(wave, phase);
        }
        graphWaves.push_back(arr);
    }
    float tablePos = evt->getFloatParamValue(IDs::oscillatorPos.toString() + String(idx));
    renderWaves(graphWaves, image, tablePos);
}

void WavetableGraph::renderWaves(std::vector<std::array<float, WAVE_GRAPH_POINTS>>& waves, Image& img, float tablePos)
{
    // version 1: boring 2d version
    img.clear(img.getBounds(), Color::black);
    for(int i = 0; i < waves.size(); i++)
    {
        float currentPos = (float)i / (float)waves.size();
        renderSingleWave(waves[(size_t)i], img, tablePos, currentPos);
    }
    
}


void WavetableGraph::renderSingleWave(std::array<float, WAVE_GRAPH_POINTS>& wave, Image& img, float tablePos, float currentPos)
{
    float colorSkew =  std::fabs(currentPos - tablePos) / 0.5f;
    Colour col = Math::clerp(Color::brightYellow, Color::black, colorSkew);
    for(int x = 0; x < img.getWidth(); x++)
    {
        float traceY = jmap(wave[(size_t)x], -1.0f, 1.0f, 0.0f, (float) WAVE_GRAPH_HEIGHT);
        for(int y = 0; y < img.getHeight(); y++)
        {
            if (std::fabs((float)y - traceY) <= 3.0f)
            {
                img.setPixelAt(x, y, col);
            }
        }
    }
}