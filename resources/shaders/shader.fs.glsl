#version 330 core

out vec4 FragColor;

in vec3 Color;
in vec3 TexCoord;

uniform sampler2D Texture;

void main()
{
	FragColor = texture(Texture, TexCoord);
}