#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../../Parameters/CircularBuffer.h"
#define GRAPH_FRAME_RATE 24
#define GRAPH_LENGTH_S 7
#define GRAPH_HEIGHT 250
#define GRAPH_DISPLAY_HEIGHT 170
#define STROKE_WIDTH 3.0f
#define STROKE_FEATHER 2.5f
#define VECTOR_GRAPH 0
class PerlinGraph : public Component, public Timer
{
private:
    EVT* const state;
    CircularBuffer<float> buf;
    Image img;
    Path gPath;
    // draws the buffer to an image bitmap
    static void renderToImage(Image& image, CircularBuffer<float>& buffer);
    // draws the buffer to a vector path (vector as in vector graphics, not like std::vector)
    static void renderToPath(Path& path, Rectangle<float>& limits, CircularBuffer<float>& buffer);
public:
    PerlinGraph(EVT* tree);
    void timerCallback() override;
    void paint(Graphics& g) override;
};