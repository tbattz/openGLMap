#version 330 core
in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox; 	// Skybox texture sampler

void main() {
	color = texture(skybox, TexCoords);
} 