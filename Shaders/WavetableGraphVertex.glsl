#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform float wavePosition;

out vec2 v_TexCoord;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * vec4(position.x, position.y, position.z, wavePosition);
    v_TexCoord = texCoord;
}
