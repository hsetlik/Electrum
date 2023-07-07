// this shader just makes stuff purple
#version 330 core
//TODO: perhaps other inputs here hmmm?
out vec4 fragColor;

void main()
{
    fragColor = vec4 (0.6f, 0.1f, 1.0f, 0.8f);
} 
