#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../../Parameters/CircularBuffer.h"
#define GRAPH_FRAME_RATE 24
#define GRAPH_LENGTH_S 7
#define GRAPH_HEIGHT 250
#define GRAPH_DISPLAY_HEIGHT 170
#define STROKE_WIDTH 3.0f
#define STROKE_FEATHER 2.5f
class PerlinGraph : public Component, public Timer
{
private:
    EVT* const state;
    CircularBuffer<float> buf;
    Image img;
    static void renderImage(Image& image, CircularBuffer<float>& buffer);
public:
    PerlinGraph(EVT* tree);
    void timerCallback() override;
    void paint(Graphics& g) override;
};