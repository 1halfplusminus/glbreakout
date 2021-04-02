#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float kernel[9];
uniform vec2 offsets[9];

void main() {
  vec3 sample[9];
  for (int i = 0; i < 9; i++) {
    sample[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
  }
  vec3 col = vec3(0.0);
  for (int i = 0; i < 9; i++) {
    col += sample[i] * kernel[i];
  }
  FragColor = vec4(col, 1.0f);
}