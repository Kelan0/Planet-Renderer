#version 400 core

in vec3 position;
in vec3 normal;
in vec2 texture;
in vec4 colour;

out vec3 p_position;
out vec3 p_normal;
out vec2 p_texture;
out vec4 p_colour;


void main(void)
{
	p_position = position;
	p_normal = normal;
	p_texture = (position.xy + vec2(1.0)) * 0.5;
	p_colour = colour;

	gl_Position = vec4(position, 1.0);
}