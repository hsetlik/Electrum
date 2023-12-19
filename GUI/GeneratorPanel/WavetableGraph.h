#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../../Parameters/Identifiers.h"
#include "../Color.h"
#include "../Utility/GLUtil.h"
#define WAVE_GRAPH_WIDTH 256
#define WAVE_GRAPH_HEIGHT 256
#define WAVE_GRAPH_POINTS 128

#define TEXTURE_W 512
#define TEXTURE_H 512

using namespace juce::gl;
class WavetableGraph : public Component, public OpenGLRenderer, public AsyncUpdater
{
public:
  EVT *const state;
  const int index;
  WavetableGraph(EVT *tree, int idx);
  ~WavetableGraph() override;
  // Component overrides
  void paint(Graphics &g) override;
  void resized() override;
  // OpenGLRenderer overrides
  void newOpenGLContextCreated() override;
  void openGLContextClosing() override;
  void renderOpenGL() override;
  // AsyncUpdater override
  void handleAsyncUpdate() override;

private:
  // helper function to generate the current texture
  char *generateTexture();
  void compileShaders();
  OpenGLContext glContext;
  std::unique_ptr<juce::OpenGLShaderProgram> shaderProgram;

  GLUtil::UniformWrapper projectionMatrix{"projectionMatrix"};
  GLUtil::UniformWrapper viewMatrix{"viewMatrix"};
  GLUtil::UniformWrapper wavePosition{"wavePosition"};

  GLuint VAO, VBO, IBO, TEX;
  std::vector<Vector3D<GLfloat>> vertices;
  std::vector<GLuint> indeces;
  // this checks the state for the current data to be graphed and converts it to
  // vertices and indeces. Does the heavy lifting of the 3D logic.
  void updateVertices();
  // this does the heavy lifting for how the texture should be drawn

  // this just easily converts betweeen a vertex's wave #/ phase index and index
  // in a vector given the total number of points
  static GLuint indexOfVertex(size_t wave, size_t sample, int numVertices);
  // projection matrix stuff
  Matrix3D<GLfloat> calculateProjectionMatrix();
  Matrix3D<GLfloat> calculateViewMatrix();
};
//=Some stuff to make texture colors easier================
namespace Texture {
void setPixel(char *buffer, int x, int y, const Colour &color);
}
