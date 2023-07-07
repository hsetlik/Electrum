// this shader's job is to project 3D mesh points through the view and projection matrices to a 2D image. or whatever
#version 330 core
layout (location = 0) 
in vec3 position;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * vec4 (position.x, position.y, position.z, 1.0);
}
