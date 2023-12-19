#pragma once
#ifndef GL_UTIL
#define GL_UTIL
#include "../../Parameters/MathUtil.h"

namespace GLUtil {
// OpenGL Uniform & Attribute Helpers ==========================================
/** Fail-safe method for creating an OpenGLShaderProgram::Uniform.

    If a uniform with the given `uniformName` is not found within the
    `shaderProgram`, then it is invalid, and nullptr is returned. If in debug mode,
    this will notify you of the naming error and provides a descriptive error message.
 */
static OpenGLShaderProgram::Uniform *
createUniform(OpenGLContext &context, OpenGLShaderProgram &shaderProgram, const String &uniformName)
{
  bool uniformNameFoundInShaderProgram =
      context.extensions.glGetUniformLocation(shaderProgram.getProgramID(),
                                              uniformName.toRawUTF8()) >= 0;

#if JUCE_DEBUG && !JUCE_DONT_ASSERT_ON_GLSL_COMPILE_ERROR
  /** If you hit this assertion, then you have attempted to create an
      OpenGLShaderProgram::Uniform with a name that is not found in the
      OpenGLShaderProgram it is linked to. Make sure your
      OpenGLShaderProgram::Uniform name matches the uniform names in your
      GLSL shader programs.
  */
  jassert(uniformNameFoundInShaderProgram);
#endif

  if (!uniformNameFoundInShaderProgram)
    return nullptr;

  return new OpenGLShaderProgram::Uniform(shaderProgram, uniformName.toRawUTF8());
}

/** Fail-safe method for creating an OpenGLShaderProgram::Attribute.

    If an attribute with the given `attributeName` is not found within the
    `shaderProgram`, then it is invalid, and nullptr is returned. If in debug mode,
    this will notify you of the naming error and provides a descriptive error message.
 */
static OpenGLShaderProgram::Attribute *createAttribute(OpenGLContext &context,
                                                       OpenGLShaderProgram &shaderProgram,
                                                       const String &attributeName)
{
  bool attributeNameFoundInShaderProgram =
      context.extensions.glGetAttribLocation(shaderProgram.getProgramID(),
                                             attributeName.toRawUTF8()) >= 0;

#if JUCE_DEBUG && !JUCE_DONT_ASSERT_ON_GLSL_COMPILE_ERROR
  /** If you hit this assertion, then you have attempted to create an
      OpenGLShaderProgram::Attribute with a name that is not found in the
      OpenGLShaderProgram it is linked to. Make sure your
      OpenGLShaderProgram::Attribute name matches the attribute names in your
      GLSL shader programs.
  */
  jassert(attributeNameFoundInShaderProgram);
#endif

  if (!attributeNameFoundInShaderProgram)
    return nullptr;

  return new OpenGLShaderProgram::Attribute(shaderProgram, attributeName.toRawUTF8());
}

/** Wrapper class that makes it easier to work with OpenGLShaderProgram::Uniform
    and OpenGLShaderProgram::Attribute. It permanently associates a particular
    name String with the uniform or attribute which is usually how Uniform and
    Attribute objects tend to be used. This makes use of these objects a bit
    cleaner instead of having the rewrite name strings in multiple places.

    For ease of use, the interface is similar to a pointer/unique_ptr with methods
    `get()`, `operator bool()`, and `operator->()`.

    I hope functionality such as this gets integrated into the juce::Uniform class
    in the future.
 */
template <class InternalType,
          InternalType *(*factoryFunction)(OpenGLContext &, OpenGLShaderProgram &, const String &)>
class OpenGLNamedIDWrapper
{
public:
  OpenGLNamedIDWrapper(const String &idName) { this->idName = idName; }

  InternalType *get() const noexcept { return uniform.get(); }

  operator bool() const noexcept { return uniform.get() != nullptr; }

  InternalType *operator->() const noexcept { return uniform.get(); }

  void connectToShaderProgram(OpenGLContext &context, OpenGLShaderProgram &shaderProgram)
  {
    uniform.reset(factoryFunction(context, shaderProgram, idName));
  }

  void disconnectFromShaderProgram() { uniform.reset(); }

private:
  String idName;
  std::unique_ptr<InternalType> uniform;
};
/** Wrapper class around OpenGLShaderProgram::Uniform for ease of use with a
    permanetly associated name. See `OpenGLNamedIDWrapper` for more information.
*/
typedef OpenGLNamedIDWrapper<OpenGLShaderProgram::Uniform, createUniform> UniformWrapper;

/** Wrapper class around OpenGLShaderProgram::Uniform for ease of use with a
    permanetly associated name. See `OpenGLNamedIDWrapper` for more information.
*/
typedef OpenGLNamedIDWrapper<OpenGLShaderProgram::Attribute, createAttribute> AttributeWrapper;

} // namespace GLUtil

#endif
