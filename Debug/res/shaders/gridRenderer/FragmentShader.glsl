#version 400 core

in vec3 p_position;
in vec3 p_normal;
in vec2 p_texture;
in vec4 p_colour;

uniform sampler2D sampler0;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform bool useTexture;
uniform vec3 cameraPosition;
uniform float gridRadius;

out vec4 colour;

void main(void)
{
	vec4 finalColour = p_colour;
	
	if (useTexture)
	{
		finalColour *= texture2D(sampler0, p_texture);
	}
	
	vec3 positionTransformed = (viewMatrix * modelMatrix * vec4(p_position, 1.0)).xyz;
	vec3 cameraTransformed = (viewMatrix * vec4(cameraPosition, 1.0)).xyz * vec3(1.0, 0.0, 1.0) + vec3(0.0, positionTransformed.y, 0.0);
	finalColour.a = 1.0 - 1.0 / ((gridRadius - 1.0) / length(cameraTransformed - positionTransformed));
	colour = finalColour;
}