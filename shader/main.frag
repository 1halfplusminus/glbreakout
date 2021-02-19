#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D image;
in vec4 SpriteColor;

void main() { 
    FragColor = SpriteColor * texture(image, TexCoord); 
}