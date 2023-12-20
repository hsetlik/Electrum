// this shader just makes stuff purple
#version 150
in vec4 gl_FragCoord;

uniform mat4 viewMatrix;

uniform float wavePosition;

uniform sampler2D u_Texture;
in vec2 v_TexCoord;

out vec4 fragColor;

void main()
{
  vec4 tColor = texture(u_Texture, v_TexCoord);
  float waveStrokeWidth = 0.01;
  fragColor = tColor;
} 
