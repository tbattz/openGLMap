#version 330 core
layout (location = 0) in vec3 position;

uniform vec3 ndc;
uniform float scale;

void main()
{	
	gl_Position = vec4((scale*position.x)+ndc[0],(scale*position.y)+ndc[1],0.0,1.0);
}