// this shader just makes stuff purple
#version 150
in vec4 gl_FragCoord;

uniform mat4 viewMatrix;

uniform float wavePosition;

out vec4 fragColor;

void main()
{
  float waveStrokeWidth = 0.01;
  fragColor = vec4(0.3, 0.3, wavePosition, 0.3);
} 
