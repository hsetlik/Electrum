// this shader's job is to project 3D mesh points through the view and projection matrices to a 2D image. or whatever
#version 330 core
layout(location = 0) in vec4 position;
layout(loaction = 1) in vec2 texCoord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec2 v_TexCoord;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * vec4 (position.x, position.y, position.z, 1.0);
    v_TexCoord = texCoord;
}
