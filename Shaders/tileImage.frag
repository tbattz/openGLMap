#version 330 core
in vec2 TexCoord;

out vec4 color;

uniform sampler2D tileTexture;
uniform float brightness;

void main() {
	color = texture(tileTexture,TexCoord)*brightness;
}