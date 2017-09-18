#version 400 core

in vec2 position;
in vec2 texture;
in vec4 colour;

out vec2 _position;
out vec2 _texture;
out vec4 _colour;

void main()
{
	_position = position;
	_texture = texture;
	_colour = colour;
	
	gl_Position = vec4(position, 0.0, 1.0);
}