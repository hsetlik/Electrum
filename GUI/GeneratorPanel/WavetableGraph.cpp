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

// this is just a helper function fot `generateTexture` that checks whether a given U coordinate is
// in range of one of hte wave traces
bool WavetableGraph::isInRange(int u, std::vector<int> &coords, int range)
{
  for (auto c : coords)
  {
    int dist = (int)std::abs(u - c);
    if (dist <= range)
      return true;
  }
  return false;
}

void WavetableGraph::generateTexture(TexBuffer &buffer)
{
  // first we figure out what U coords should have wave outlines drawn
  const int numWaves = (int)(vData.size() / WAVE_GRAPH_POINTS);
  std::vector<int> waveCoords;
  int uPos = 0;
  const int spacing = (int)TEXTURE_H / numWaves;
  while (uPos <= TEXTURE_H)
  {
    waveCoords.push_back(uPos);
    uPos += spacing;
  }
  const Colour bkgnd = Color::black.withAlpha(0.3f);
  const Colour trace = Color::brightSeafoam.withAlpha(0.8f);
  for (int u = 0; u < TEXTURE_H; u++)
  {
    bool isTrace = isInRange(u, waveCoords, 2);
    auto col = isTrace ? trace : bkgnd;
    for (int v = 0; v < TEXTURE_W; v++)
    {
      Texture::setPixel(buffer, u, v, col);
      // draw a white outline around the edge of the texture
      if (u == 0 || u == TEXTURE_H - 1 || v == 0 || v == TEXTURE_W - 1)
      {
        Texture::setPixel(buffer, u, v, Colours::white);
      }
    }
  }
}

//==========GL overrides===========

void WavetableGraph::newOpenGLContextCreated()
{
  compileShaders();
  updateVertices();

  // double languageVersion = OpenGLShaderProgram::getLanguageVersion();
  //  DLog::log("Using GLSL version: " + String(languageVersion));

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

void WavetableGraph::openGLContextClosing()
{
  // clean up the textures
  glDeleteTextures(1, &TEX);
}

void WavetableGraph::renderOpenGL()
{
  jassert(OpenGLHelpers::isContextActive());
  // Scale viewport
  const float renderingScale = (float)glContext.getRenderingScale();
  glViewport(0, 0, (int)renderingScale * getWidth(), (int)renderingScale * getHeight());
  checkGLError("Failed to set viewport");

  // Set background color
  juce::OpenGLHelpers::clear(Color::maroon);
  // Select shader program
  shaderProgram->use();
  checkGLError("Failed to select shader program");
  // Setup the Uniforms for use in the Shader
  if (projectionMatrix)
  {
    projectionMatrix->setMatrix4(calculateProjectionMatrix().mat, 1, false);
    checkGLError("Failed to set projection matrix");
  }
  if (viewMatrix)
  {
    viewMatrix->setMatrix4(calculateViewMatrix().mat, 1, false);
    checkGLError("Failed to set view matrix");
  }
  if (wavePosition)
  {
    GLfloat val = (GLfloat)state->getLeadingVoiceOscPosition(index);
    wavePosition->set(val);
    checkGLError("Failed to set wave position");
  }
  if (texSlot)
  {
    texSlot->set(index);
    checkGLError("Failed to set texture slot");
  }
  glBindTexture(GL_TEXTURE_2D, TEX);
  checkGLError("Failed to bind texture");
  glContext.extensions.glActiveTexture(GL_TEXTURE0 + (GLuint)index);
  checkGLError("Failed to set active texture");
  // Draw Vertices
  glContext.extensions.glBindVertexArray(VAO);
  checkGLError("Failed to bind vertexArray");
  glDrawElements(GL_TRIANGLES, (GLsizei)indeces.size(), GL_UNSIGNED_INT, nullptr);
  checkGLError("Failed to draw elements");
  glContext.extensions.glBindVertexArray(0);
  checkGLError("Failed to unbind vertex array");
  glBindTexture(GL_TEXTURE_2D, 0);
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
  vData.clear();
  auto baseWaves = state->getAudioData()->getBaseWaves(index);
  // now add the vertices for each wave
  for (size_t w = 0; w < baseWaves.size(); w++)
  {
    auto &wave = baseWaves[w];
    auto zPhase = (float)w / (float)vData.size();
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
  const int numVertices = (int)vData.size();
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
  float w = 1.0f / 0.8f;
  float h = w * getLocalBounds().toFloat().getAspectRatio(false);
  float yDist = 20.0f;
  return Matrix3D<GLfloat>::fromFrustum(-w, w, -h, h, 4.0f, yDist);
}

Matrix3D<GLfloat> WavetableGraph::calculateViewMatrix()
{
  float scaleFactor = 3.0f;
  auto scale = Matrix3D<GLfloat>(AffineTransform::scale(scaleFactor, scaleFactor));
  auto angleX = MathConstants<float>::pi * 0.0f;
  auto angleY = MathConstants<float>::pi * 0.0f;
  auto angleZ = MathConstants<float>::pi * 0.05f;
  auto rotation = Matrix3D<GLfloat>::rotation(
      Vector3D<GLfloat>(angleX / scaleFactor, angleY / scaleFactor, angleZ / scaleFactor));
  auto translate = Matrix3D<GLfloat>::fromTranslation(Vector3D<GLfloat>(-0.5, -0.8f, -20.0f));
  auto out = scale * rotation * translate;
  return out;
}
