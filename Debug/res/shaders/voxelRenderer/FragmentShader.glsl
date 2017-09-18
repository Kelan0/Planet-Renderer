#version 400 core

struct Light
{
	vec3 position;
	vec3 colour;
	vec3 attenuation;
	vec3 direction;
	float innerRadius;
	float outerRadius;
	int type;
};

const int numLights = 1;
const int POINT_LIGHT = 0;
const int SPOT_LIGHT = 1;
const int DIRECTIONAL_LIGHT = 2;

in vec3 p_position;
in vec3 p_normal;
in vec2 p_texture;
in vec4 p_colour;
in float flogz;

uniform float Fcoef;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform sampler2D sampler0;
uniform bool useTexture;
uniform bool useLighting;
uniform vec3 cameraPosition;
uniform Light lights[numLights];

out vec4 colour;

void main(void)
{
	vec4 finalColour = p_colour;
	
	if (useTexture)
	{
		finalColour *= texture2D(sampler0, p_texture);
	}
	
	//finalColour = vec4(((p_normal + vec3(1.0)) / 2.0), 1.0);
	
	if (useLighting)
	{
		vec3 lightColour = vec3(0.0);
		
		vec3 worldPos = (modelMatrix * vec4(p_position, 1.0)).xyz;
		vec3 cameraPosition = (viewMatrix * vec4(cameraPosition, 1.0)).xyz;
		vec3 surfacePosition = (viewMatrix * modelMatrix * vec4(p_position, 1.0)).xyz;
		vec3 surfaceNormal = normalize(transpose(inverse(mat3(viewMatrix * modelMatrix))) * p_normal);
		//vec3 surfaceNormal = normalize(cross(dFdx(surfacePosition), dFdy(surfacePosition)));
		
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
	
	gl_FragDepth = log2(flogz) * (Fcoef / 2.0);
	colour = vec4(finalColour);
}