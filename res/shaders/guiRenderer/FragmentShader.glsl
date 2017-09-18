#version 400 core

in vec3 p_position;
in vec3 p_normal;
in vec2 p_texture;
in vec4 p_colour;

uniform sampler2D sampler0;
uniform bool useTexture;

out vec4 colour;

void main(void)
{
	vec4 finalColour;
	
	if (useTexture)
	{
		finalColour = p_colour * texture2D(sampler0, p_texture);
	} else
	{
		finalColour = p_colour;
	}
	
	colour = finalColour;
}