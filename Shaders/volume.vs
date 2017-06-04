#version 330 core
layout (location = 0) in vec3 position;
  
uniform mat4 view;
uniform mat4 projection;

uniform vec3 color;

out vec4 vertexColor;

void main()
{
    gl_Position = projection * view * vec4(position, 1.0);
    vertexColor = vec4(color, 0.5);
}
