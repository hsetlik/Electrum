#include "WavetableGraph.h"

WavetableGraph::WavetableGraph(EVT* tree, int idx) : state(tree), index(idx)
{
   glContext.setOpenGLVersionRequired(juce::OpenGLContext::OpenGLVersion::openGL3_2); 
   glContext.setRenderer(this);
   glContext.attachTo(*this);
   glContext.setContinuousRepainting(true);
}

WavetableGraph::~WavetableGraph()
{
    glContext.setContinuousRepainting(false);
    glContext.detach();
}
// component overrides
void WavetableGraph::paint(Graphics& g) 
{

}

void WavetableGraph::resized() 
{

}
// AsyncUpdater override
void WavetableGraph::handleAsyncUpdate() 
{

}

//shader loading
void WavetableGraph::compileShaders()
{

}

//==========GL overrides===========

void WavetableGraph::newOpenGLContextCreated() 
{

}

void WavetableGraph::openGLContextClosing() 
{

}

void WavetableGraph::renderOpenGL() 
{

}


