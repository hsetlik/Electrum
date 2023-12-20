// this shader just makes stuff purple
#version 330 core
in vec4 gl_FragCoord;

uniform float wavePosition;
uniform sampler2D u_Texture;

in vec2 v_TexCoord;

out vec4 fragColor;

void main()
{
  vec4 tColor = texture(u_Texture, v_TexCoord);
  float waveStrokeWidth = 0.01;
  fragColor = vec4(tColor.x, tColor.y, tColor.z, 0.7);
} 
