#version 400 core

in vec3 p_position;
in vec3 p_normal;
in vec2 p_texture;
in vec4 p_colour;

uniform sampler2D sampler0;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform bool useTexture;
uniform bool useLighting;
uniform vec3 cameraPosition;

out vec4 colour;

void main(void)
{
	vec4 finalColour = p_colour;
	
	if (useTexture)
	{
		finalColour *= texture2D(sampler0, p_texture);
	}
	
	if (useLighting)
	{
		vec3 lightColour = vec3(0.0);
		
		vec3 cameraPosition = (viewMatrix * vec4(cameraPosition, 1.0)).xyz;
		vec3 surfacePosition = (viewMatrix * modelMatrix * vec4(p_position, 1.0)).xyz;
		vec3 surfaceNormal = normalize(transpose(inverse(mat3(viewMatrix * modelMatrix))) * p_normal);
		
		for (int i = 0; i < numLights; i++)
		{
			Light light = lights[i];
			
			vec3 lightPosition = (viewMatrix * vec4(light.position, 1.0)).xyz;
			
			vec3 dirToLight = normalize(lightPosition - surfacePosition);
			vec3 dirToCamera = normalize(cameraPosition - surfacePosition);
			
			float f = max(dot(surfaceNormal, dirToLight), 0.0);
			lightColour += light.colour * f;
		}
		
		lightColour = clamp(lightColour, (1.0 / 200.0), 1.0);
		finalColour *= vec4(lightColour, 1.0);
	}
	
	colour = finalColour;
}