#include "PerlinGraph.h"

PerlinGraph::PerlinGraph(EVT* tree) : 
state(tree),
buf(GRAPH_FRAME_RATE * GRAPH_LENGTH_S),
img(Image::PixelFormat::RGB, GRAPH_FRAME_RATE * GRAPH_LENGTH_S, GRAPH_HEIGHT, true)
{
    startTimerHz(GRAPH_FRAME_RATE);
}
void PerlinGraph::timerCallback() 
{
    TRACE_COMPONENT();
    buf.push(state->perlinValue());
    renderImage(img, buf);
    repaint();
}

void PerlinGraph::paint(Graphics& g)
{
    g.drawImage(img, getLocalBounds().toFloat());
}

//====================================================================================
void PerlinGraph::renderImage(Image& image, CircularBuffer<float>& buffer)
{
    for(int x = 0; x < GRAPH_FRAME_RATE * GRAPH_LENGTH_S; x++)
    {
        float rowY = buffer[(size_t) x] * (float)GRAPH_HEIGHT;
        for(int y = 0; y < GRAPH_HEIGHT; y++)
        {
            float yDist = fabs(rowY - (float)y);
            auto col = Color::black;
            if (yDist <= STROKE_WIDTH)
            {
                col = Color::brightYellow;
            }
            else if (yDist <= STROKE_WIDTH + STROKE_FEATHER)
            {
                float t = (yDist - STROKE_WIDTH) / STROKE_FEATHER;
                col = Math::clerp(Color::brightYellow, Color::black, t);
            }
            image.setPixelAt(x, y, col);
        }
    }
}