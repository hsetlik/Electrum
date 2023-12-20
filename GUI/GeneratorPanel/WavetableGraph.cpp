#include "WavetableGraph.h"
#include "juce_opengl/opengl/juce_gl.h"
//============================================================================
void Texture::setPixel(TexBuffer &buffer, int x, int y, const Colour &color)
{
  const int idx = ((y * TEXTURE_W) + x) * 4;
  buffer[idx] = (char)color.getRed();
  buffer[idx + 1] = (char)color.getGreen();
  buffer[idx + 2] = (char)color.getBlue();
  buffer[idx + 3] = (char)color.getAlpha();
}
//============================================================================
WavetableGraph::WavetableGraph(EVT *tree, int idx) : state(tree), index(idx)
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
  // glDeleteTextures(1, &TEX);
}
// component overrides
void WavetableGraph::paint(Graphics &) {}

void WavetableGraph::resized() {}
// AsyncUpdater override
void WavetableGraph::handleAsyncUpdate() { repaint(); }

// shader loading
void WavetableGraph::compileShaders()
{
  auto shaderProgramAttempt = std::make_unique<OpenGLShaderProgram>(glContext);
  if (shaderProgramAttempt->addVertexShader({Shaders::WavetableGraphVertex_glsl}) &&
      shaderProgramAttempt->addFragmentShader({Shaders::WavetableGraphFragment_glsl}) &&
      shaderProgramAttempt->link())
  {
    projectionMatrix.disconnectFromShaderProgram();
    viewMatrix.disconnectFromShaderProgram();
    wavePosition.disconnectFromShaderProgram();

    shaderProgram.reset(shaderProgramAttempt.release());

    projectionMatrix.connectToShaderProgram(glContext, *shaderProgram);
    viewMatrix.connectToShaderProgram(glContext, *shaderProgram);
    wavePosition.connectToShaderProgram(glContext, *shaderProgram);

  } else
  {
    DLog::log("Failed to compile shaders!");
  }
}
//=================================================
void WavetableGraph::generateTexture(TexBuffer &buffer)
{
  float xPhase, yPhase;
  for (int x = 0; x < TEXTURE_W; x++)
  {
    xPhase = (float)x / (float)TEXTURE_W;
    auto c1 = Math::clerp(Color::darkBkgnd, Color::offWhite, xPhase);
    for (int y = 0; y < TEXTURE_H; y++)
    {
      yPhase = (float)y / (float)TEXTURE_H;
      auto c2 = Math::clerp(Color::darkSeaGreen, c1, yPhase);
      Texture::setPixel(buffer, x, y, c2);
    }
  }
}

//==========GL overrides===========

void WavetableGraph::newOpenGLContextCreated()
{
  compileShaders();
  updateVertices();

  double languageVersion = OpenGLShaderProgram::getLanguageVersion();
  DLog::log("Using GLSL version: " + String(languageVersion));

  // Generate opengl vertex and index objects
  // ==========================================
  glContext.extensions.glGenVertexArrays(1, &VAO); // Vertex Array Object
  glContext.extensions.glGenBuffers(1, &VBO);      // Vertex Buffer Object
  glContext.extensions.glGenBuffers(1, &IBO);      // Index Buffer Object
  // set up texture stuff
  glGenTextures(1, &TEX);
  checkGLError("Failed to generate textures");
  glBindTexture(GL_TEXTURE_2D, TEX);
  checkGLError("Failed to bind texture");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  checkGLError("Failed to set texture filters");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  checkGLError("Failed to set texture wrap");
  // now generate the actual texture bytes
  generateTexture(currentTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, TEXTURE_W, TEXTURE_H, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               currentTexture);
  checkGLError("Failed glTexImage2D");
  glBindTexture(GL_TEXTURE_2D, 0);
  checkGLError("Failed to bind texture 0");

  // bind the texture
  // glBindTexture(GL_TEXTURE_2D, TEX);
  glContext.extensions.glActiveTexture(GL_TEXTURE0 + (GLuint)index);

  checkGLError("Failed to set active texture");
  glContext.extensions.glBindVertexArray(VAO);
  checkGLError("Failed to bind vertex array");

  // Fill VBO buffer with vertices array
  glContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, VBO);
  checkGLError("Failed to bind vertex buffer");
  glContext.extensions.glBufferData(GL_ARRAY_BUFFER, (long)sizeof(GLfloat) * (long)vData.size() * 5,
                                    vData.data(), GL_DYNAMIC_DRAW);

  checkGLError("Failed to set vertex buffer data");
  // fill IBO buffer with indeces array
  glContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  checkGLError("Failed to bind index buffer");
  glContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                    (long)sizeof(GLuint) * (long)indeces.size(), indeces.data(),
                                    GL_DYNAMIC_DRAW);

  checkGLError("Failed to set index buffer data");
  // Define that our vertices are laid out as groups of 3 GLfloats
  glContext.extensions.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPoint),
                                             nullptr);
  glContext.extensions.glEnableVertexAttribArray(0);
  checkGLError("Failed position glVertexAttribPointer");
  glContext.extensions.glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPoint),
                                             nullptr);
  glContext.extensions.glEnableVertexAttribArray(1);
  checkGLError("Failed texture coordinate glVertexAttribPointer");
}

void WavetableGraph::openGLContextClosing() {}

void WavetableGraph::renderOpenGL()
{
  jassert(OpenGLHelpers::isContextActive());

  // Scale viewport
  const float renderingScale = (float)glContext.getRenderingScale();
  glViewport(0, 0, (int)renderingScale * getWidth(), (int)renderingScale * getHeight());

  // Set background color
  juce::OpenGLHelpers::clear(Color::maroon);
  // Select shader program
  shaderProgram->use();
  // Setup the Uniforms for use in the Shader
  if (projectionMatrix)
    projectionMatrix->setMatrix4(calculateProjectionMatrix().mat, 1, false);
  if (viewMatrix)
    viewMatrix->setMatrix4(calculateViewMatrix().mat, 1, false);
  if (wavePosition)
  {
    GLfloat val = (GLfloat)state->getLeadingVoiceOscPosition(index);
    wavePosition->set(val);
  }
  if (texSlot)
  {
    texSlot->set(index);
  }

  // Draw Vertices
  glContext.extensions.glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, (GLsizei)indeces.size(), GL_UNSIGNED_INT, nullptr);
  glContext.extensions.glBindVertexArray(0);
}

//===========Vertex generation======================

GLuint WavetableGraph::indexOfVertex(size_t wave, size_t sample, int numVertices)
{
  size_t offset = (wave * WAVE_GRAPH_POINTS) + sample;
  return (GLuint)((size_t)numVertices - offset);
}
void WavetableGraph::updateVertices()
{
  DLog::log("Updating vertices for graph #" + String(index + 1));
  const float xMax = 1.5f;
  const float xMin = -1.5f;
  const float yMax = 1.0f;
  const float yMin = 0.0f;
  const float zMax = 1.0f;
  const float zMin = 0.0f;
  vertices.clear();
  auto baseWaves = state->getAudioData()->getBaseWaves(index);
  // now add the vertices for each wave
  for (size_t w = 0; w < baseWaves.size(); w++)
  {
    auto &wave = baseWaves[w];
    auto zPhase = (float)w / (float)vertices.size();
    for (int s = 0; s < WAVE_GRAPH_POINTS; s++)
    {
      float xPhase = (float)s / (float)WAVE_GRAPH_POINTS;
      float sample = WaveUtil::valueAtPhase(wave, 1.0f - xPhase);
      float xPos = Math::flerp(xMin, xMax, 1.0f - xPhase);
      float yPos = Math::flerp(yMin, yMax, sample);
      float zPos = Math::flerp(zMin, zMax, zPhase);
      // if(index == 0)
      // {
      //   DLog::log("Point #" + String(s) + " is at: " + String(xPos) + ", " +
      //   String(yPos) + ", " + String(zPos));
      // }
      vertices.push_back({xPos, yPos, zPos});
      VertexPoint p;
      p[0] = xPos;
      p[1] = yPos;
      p[2] = zPos;
      p[3] = xPos;
      p[4] = yPos;
      vData.push_back(p);
    }
  }
  // now time to figure out the indeces
  const int numVertices = (int)vertices.size();
  indeces.clear();
  for (size_t w = 1; w < baseWaves.size(); w++)
  {
    size_t lowerWave = w - 1;
    size_t upperWave = w;
    // now loop through the points and draw 2 triangles between the adjacent
    // wave points
    for (size_t i = 1; i < WAVE_GRAPH_POINTS; i++)
    {
      size_t lowerX = i - 1;
      size_t upperX = i;
      // grip the indeces we need
      const GLuint ll = indexOfVertex(lowerWave, lowerX, numVertices);
      const GLuint lu = indexOfVertex(lowerWave, upperX, numVertices);
      const GLuint ul = indexOfVertex(upperWave, lowerX, numVertices);
      const GLuint uu = indexOfVertex(upperWave, upperX, numVertices);
      // first triangle
      indeces.push_back(lu);
      indeces.push_back(uu);
      indeces.push_back(ll);
      // second triangle
      indeces.push_back(uu);
      indeces.push_back(ul);
      indeces.push_back(ll);
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
  float scaleFactor = 4.0f;
  auto scale = Matrix3D<GLfloat>(AffineTransform::scale(scaleFactor, scaleFactor));
  auto angleX = MathConstants<float>::pi * 0.0f;
  auto angleY = MathConstants<float>::pi * 0.0f;
  auto angleZ = MathConstants<float>::pi * 0.05f;
  auto rotation = Matrix3D<GLfloat>::rotation(
      Vector3D<GLfloat>(angleX / scaleFactor, angleY / scaleFactor, angleZ / scaleFactor));
  auto translate = Matrix3D<GLfloat>::fromTranslation(Vector3D<GLfloat>(0.0f, -0.5f, -20.0f));
  auto out = scale * rotation * translate;
  return out;
}
