#version 400 core

in vec3 position;
in vec3 normal;
in vec2 texture;
in vec4 colour;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec3 p_position;
out vec3 p_normal;
out vec2 p_texture;
out vec4 p_colour;

void main(void)
{
	 p_position = position;
	 p_normal = normal;
	 p_texture = texture;
	 p_colour = colour;

	//MVP = projectionMatrix * viewMatrix * modelMatrix
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}