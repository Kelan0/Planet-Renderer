#version 400 core

in vec3 p_position;
in vec3 p_normal;
in vec2 p_texture;
in vec4 p_colour;

uniform sampler2D sampler0;

out vec4 colour;

void main(void)
{
	vec3 texColour = texture2D(sampler0, p_texture).rgb;
	float brightness = 0.2126 * texColour.r + 0.7152 * texColour.g + 0.0722 * texColour.b;
	float gamma = 1.0;
	float exposure = 1.0; //higher exposure captures more light

	
	vec3 toneMappedColour = vec3(1.0) - exp(-texColour * exposure);
	vec3 finalColour = pow(toneMappedColour, vec3(1.0 / gamma));
	
	colour = vec4(finalColour, 1.0);
}