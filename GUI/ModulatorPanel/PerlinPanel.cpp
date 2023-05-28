#include "PerlinPanel.h"
PerlinPanel::PerlinPanel(EVT* tree) : 
state(tree),
sFreq(tree, IDs::perlinFreq.toString()),
sLac(tree, IDs::perlinLacunarity.toString()),
sOct(tree, IDs::perlinOctaves.toString()),
graph(state),
sComp(state, IDs::perlinSource.toString())
{
    addAndMakeVisible(&sFreq);
    addAndMakeVisible(&sLac);
    addAndMakeVisible(&sOct);
    addAndMakeVisible(&graph);
    addAndMakeVisible(&sComp);

}
void PerlinPanel::resized() 
{
    auto lBounds = getLocalBounds().toFloat();
    auto paramArea = lBounds.removeFromTop(lBounds.getHeight() / 5.0f);
    auto pWidth = paramArea.getWidth() / 4.0f;
    sFreq.setBounds(paramArea.removeFromLeft(pWidth).toNearestInt());
    sLac.setBounds(paramArea.removeFromLeft(pWidth).toNearestInt());
    sOct.setBounds(paramArea.removeFromLeft(pWidth).toNearestInt());
    auto sourceArea = lBounds.removeFromRight(lBounds.getWidth() * 0.2f);
    sComp.setBounds(sourceArea.removeFromTop(sourceArea.getWidth()).toNearestInt());

    graph.setBounds((int)lBounds.getX(), (int)lBounds.getY(), GRAPH_FRAME_RATE * GRAPH_LENGTH_S, GRAPH_DISPLAY_HEIGHT);
}
void PerlinPanel::paint(Graphics& g) 
{
    g.fillAll(Color::gainsboro);
}