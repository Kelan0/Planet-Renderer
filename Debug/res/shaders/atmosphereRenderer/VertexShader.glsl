#version 400 core

const float PI = 3.14159265358979323846264338327950288;
in vec3 position;
in vec3 normal;
in vec2 texture;
in vec4 colour;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float Fcoef;
uniform float near;

out vec3 p_position;
out vec3 p_normal;
out vec2 p_texture;
out vec4 p_colour;
out float flogz;

uniform float innerRadius;
uniform float outerRadius;
uniform float scaleDepth;
uniform float reyleigh;
uniform float mei;
uniform float ESun;
uniform int numSamples;
uniform vec3 wavelength;
uniform vec3 planetPos;
uniform vec3 cameraPos;
uniform vec3 lightPos;

invariant out vec3 vertexToCamera;

out vec4 dgl_SecondaryColor;
out vec4 dgl_Color;

float scaleAngle(float fCos)
{
	float x = 1.0 - fCos;
	return scaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}


void main(void)
{
	float scale = 1.0 / (outerRadius - innerRadius);
	float scaleOverScaleDepth = scale / scaleDepth;
    //gg_FrontColor = vec3(1.0, 0.0, 0.0);
    //gg_FrontSecondaryColor = vec3(0.0, 1.0, 0.0);

	// Get the ray from the camera to the vertex, and its length (which is the far point of the ray passing through the atmosphere)
	vec3 vertexPos = position;
	vec3 ray = vertexPos - cameraPos;
	float rayLength = length(ray);
	ray /= rayLength;

	float cameraHeight = length(cameraPos - planetPos);
	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 v3Start = cameraPos;
	float fHeight = length(v3Start);
	float fDepth = exp(scaleOverScaleDepth * (innerRadius - cameraHeight));
	float fStartAngle = dot(ray, v3Start) / fHeight;
	float fStartOffset = fDepth*scaleAngle(fStartAngle);

	// Initialize the scattering loop variables
	vec4 colour1 = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 colour2 = vec4(0.0, 0.0, 0.0, 0.0);

	float fSampleLength = rayLength / float(numSamples);
	float fScaledLength = fSampleLength * scale;
	vec3 v3SampleRay = ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.2, 0.1, 0.0);
	for(int i = 0; i < numSamples; i++)
	{
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(scaleOverScaleDepth * (innerRadius - fHeight));
		float fLightAngle = dot(lightPos, v3SamplePoint) / fHeight;
		float fCameraAngle = dot(ray, v3SamplePoint) / fHeight;
		float fScatter = (fStartOffset + fDepth*(scaleAngle(fLightAngle) - scaleAngle(fCameraAngle)));
		vec3 v3Attenuate = exp(-fScatter * (wavelength * (reyleigh * 4.0 * PI) + (mei * 4.0 * PI)));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
	colour2.rgb = v3FrontColor;// * (mei * ESun);
	colour1.rgb = v3FrontColor;// * (wavelength * (reyleigh * ESun));
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
	gl_Position.z = log2(max(near, 1.0 + gl_Position.w)) * Fcoef - 1.0;
	
	vertexToCamera = cameraPos - vertexPos;

	dgl_SecondaryColor = colour2;
	dgl_Color = colour1;
	
	flogz = 1.0 + gl_Position.w;
}