#pragma once
#include "../../Parameters/Identifiers.h"
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"
#include "../Utility/GLUtil.h"
#define WAVE_GRAPH_WIDTH 256
#define WAVE_GRAPH_HEIGHT 256
#define WAVE_REFRESH_HZ 24
#define WAVE_GRAPH_POINTS 128

class WavetableGraph : 
public Component,
public OpenGLRenderer,
public AsyncUpdater
{
public:
    EVT* const state;
    const int index;
    WavetableGraph(EVT* tree, int idx);
    ~WavetableGraph() override;
    // Component overrides
    void paint(Graphics& g) override;
    void resized() override;
    // OpenGLRenderer overrides
    void newOpenGLContextCreated() override;
    void openGLContextClosing() override;
    void renderOpenGL() override;
    // AsyncUpdater override
    void handleAsyncUpdate() override;
private:
    // Grip the shader code, figure shit out
    void compileShaders();
    OpenGLContext glContext;

    GLUtil::UniformWrapper projectionMatrix { "projectionMatrix" };
    GLUtil::UniformWrapper viewMatrix { "viewMatrix" };
    
    

};