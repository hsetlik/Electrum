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
#if VECTOR_GRAPH
    auto lBounds = getLocalBounds().toFloat();
    renderToPath(gPath, lBounds, buf);
#else
    renderToImage(img, buf);
#endif
    repaint();
}

void PerlinGraph::paint(Graphics& g)
{
#if VECTOR_GRAPH
    g.fillAll(Color::darkSlateGray);
    g.setColour(Color::aquamarine);
    PathStrokeType pst(1.5f);
    g.strokePath(gPath, pst);
#else
    g.drawImage(img, getLocalBounds().toFloat());
#endif
}

//====================================================================================
void PerlinGraph::renderToImage(Image& image, CircularBuffer<float>& buffer)
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


void PerlinGraph::renderToPath(Path& path, Rectangle<float>& limits, CircularBuffer<float>& buffer)
{
    const float fHeight = (float)(limits.getHeight() - 3.0f);
    const float dX = limits.getWidth() / (float)(GRAPH_FRAME_RATE * GRAPH_LENGTH_S);
    const float yMax = limits.getBottom() - 3.0f;
    path.clear();
    path.startNewSubPath(limits.getX(), yMax - (fHeight * buffer[0]));
    for(size_t i = 1; i < GRAPH_FRAME_RATE * GRAPH_LENGTH_S; i++)
    {
        path.lineTo(dX * (float)i, yMax - (fHeight * buffer[i]));
    }
}