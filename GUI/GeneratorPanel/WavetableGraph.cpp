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
    auto shaderProgramAttempt = std::make_unique<OpenGLShaderProgram>(glContext);
     if (shaderProgramAttempt->addVertexShader ({ Shaders::WavetableGraphVertex_glsl })
        && shaderProgramAttempt->addFragmentShader ({ Shaders::WavetableGraphFragment_glsl })
        && shaderProgramAttempt->link())
    {
        projectionMatrix.disconnectFromShaderProgram();
        viewMatrix.disconnectFromShaderProgram();
        
        shaderProgram.reset(shaderProgramAttempt.release());
        
        projectionMatrix.connectToShaderProgram (glContext, *shaderProgram);
        viewMatrix.connectToShaderProgram (glContext, *shaderProgram);
    }
}

//==========GL overrides===========

void WavetableGraph::newOpenGLContextCreated() 
{
    compileShaders();
    updateVertices();

    // Generate opengl vertex objects ==========================================
    glContext.extensions.glGenVertexArrays(1, &VAO); // Vertex Array Object
    glContext.extensions.glGenBuffers (1, &VBO);     // Vertex Buffer Object
    
    glContext.extensions.glBindVertexArray (VAO);
    
    // Fill VBO buffer with vertices array
    glContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, VBO);
    glContext.extensions.glBufferData (GL_ARRAY_BUFFER,
                                           sizeof (GLfloat) * vertices.size() * 3,
                                           vertices.data(),
                                           GL_STATIC_DRAW);


    // Define that our vertices are laid out as groups of 3 GLfloats
    glContext.extensions.glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE,
                                                    3 * sizeof (GLfloat), NULL);
    glContext.extensions.glEnableVertexAttribArray (0);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Show wireframe
}

void WavetableGraph::openGLContextClosing() 
{

}

void WavetableGraph::renderOpenGL() 
{
    jassert (OpenGLHelpers::isContextActive());
    
    // Scale viewport
    const float renderingScale = (float) glContext.getRenderingScale();
    glViewport (0, 0, (int)renderingScale * getWidth(), (int)renderingScale * getHeight());

    // Set background color
    juce::OpenGLHelpers::clear(Color::darkSlateGray);
    // Select shader program
    shaderProgram->use();
    // Setup the Uniforms for use in the Shader
    if (projectionMatrix)
        projectionMatrix->setMatrix4 (calculateProjectionMatrix().mat, 1, false);
    if (viewMatrix)
        viewMatrix->setMatrix4 (calculateViewMatrix().mat, 1, false);
    
    // Draw Vertices
    glContext.extensions.glBindVertexArray (VAO);
    glDrawArrays (GL_TRIANGLES, 0, (int) vertices.size());
    glContext.extensions.glBindVertexArray (0);
}

//===========Vertex generation======================
void WavetableGraph::updateVertices()
{
    const float xMax = 10.0f;
    const float yMax = 10.0f;
    const float zMax = 10.0f;
    vertices.clear();
    auto baseWaves = state->getAudioData()->getBaseWaves(index);
    //now add the vertices for each wave
    for(int w = 0; w < vertices.size(); w++)
    {
        auto& wave = baseWaves[w];
        auto zPhase = (float)w / (float)vertices.size();
        for(int s = 0; s < WAVE_GRAPH_POINTS; s++)
        {
            float xPhase = (float)s / (float)WAVE_GRAPH_POINTS;
            float sample = WaveUtil::valueAtPhase(wave, xPhase);
            float yPhase = jmap(sample, -1.0f, 1.0f, 0.0f, 1.0f);
            vertices.push_back({xPhase * xMax, yPhase * yMax, zPhase * zMax});
        }
    }

}

//===========Matrix generation======================
Matrix3D<GLfloat> WavetableGraph::calculateProjectionMatrix()
{
    float w = 1.0f / 0.6f;
    float h = w * getLocalBounds().toFloat().getAspectRatio(false);
    
    return Matrix3D<GLfloat>::fromFrustum(-w, w, -h, h, 4.0f, 20.0f);
}

Matrix3D<GLfloat> WavetableGraph::calculateViewMatrix()
{
    float scaleFactor = 2.0f;
    auto scale = Matrix3D<GLfloat>(AffineTransform::scale(scaleFactor * 3.0f, scaleFactor));
    auto angleX = MathConstants<float>::pi * 0.0f;
    auto angleY = MathConstants<float>::pi * 0.0f;
    auto angleZ = MathConstants<float>::pi * 0.0f;
    auto rotation = Matrix3D<GLfloat>::rotation(Vector3D<GLfloat>(angleX / scaleFactor,
                                                                              angleY / scaleFactor,
                                                                              angleZ / scaleFactor));
    auto translate = Matrix3D<GLfloat>::fromTranslation(Vector3D<GLfloat> (0.0f, 0.0f, -10.0f));
    auto out = scale * rotation * translate;
    return out;
}



