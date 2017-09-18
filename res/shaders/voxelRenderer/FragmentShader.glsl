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

const float PI = 3.14159265358979323846264338327950288;
const int nSamples = 16;
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
uniform mat4 projectionMatrix;
uniform sampler2D sampler0;
uniform bool useTexture;
uniform bool useLighting;
uniform bool hasAtmosphere;
uniform vec3 cameraPosition;
uniform Light lights[numLights];

uniform vec3  v3CameraPos;      	// The camera's current position
uniform vec3  v3PlanetPos;      	// The planet's current position
uniform vec3  v3LightDir;       	// Direction vector to the light source
uniform vec3  v3InvWavelength;  	// 1 / pow(wavelength, 4) for RGB
uniform float fOuterRadius;    		// The outer (atmosphere) radius
uniform float fInnerRadius;    		// The inner (planetary) radius
uniform float fESun;         		// Kr * ESun
uniform float fKr;          		// Kr
uniform float fKm;          		// Kr
uniform float fScaleDepth; 			// The height where the average atmospheric density is found
uniform float g;
uniform float fAtmosphericDensity;

out vec4 colour;

bool getSphereIntersection(vec3 v3StartPos, vec3 v3Ray, float fRadius, inout float fNear, inout float fFar)
{
	float fHeight2 = dot(v3StartPos - v3PlanetPos, v3StartPos - v3PlanetPos);
	float B = 2.0 * dot(v3StartPos, v3Ray);
	float C = fHeight2 - (fRadius * fRadius);
	float fDet = B * B - 4.0 * C;
	
	if (fDet <= 0.0)
	{
		fNear = 1.0;
		fFar = -1.0;
	} else
	{
		fNear = 0.5 * (-B - sqrt(fDet));
		fFar = 0.5 * (-B + sqrt(fDet));
	}
	return fFar > fNear;
}

float density(float fHeight)
{
	float fScaleOverScaleDepth = 1.0 / ((fOuterRadius - fInnerRadius) * fScaleDepth);
	//scaling the curve, such that it crosses the y-axis at 1, and the x-axis at the atmosphere height (fOuterRadius - fInnerRadius)
	float fCurDensity = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
	float fMinDensity = exp(fScaleOverScaleDepth * (fInnerRadius - fOuterRadius));
	float fDensity = (fCurDensity - fMinDensity) / (1.0 - fMinDensity);
	
	return max(0.0, fDensity);
}

float optic(vec3 start, vec3 end) //gets the average density between start and end
{
	vec3 step = (end - start) / float(nSamples);
	vec3 v = start + step * 0.5;
	
	float sum = 0.0;
	for (int i = 0; i < nSamples; i++)
	{
		sum += density(distance(v, v3PlanetPos));
		v += step;
	}
	
	sum *= length(step) / (fOuterRadius - fInnerRadius);
	
	return sum;
}

void calculateLighting(inout vec4 finalColour)
{
	if (useTexture)
	{
		finalColour *= texture2D(sampler0, p_texture);
	}
	
	if (useLighting)
	{
		vec3 lightColour = vec3(0.0);
		
		vec3 worldPos = (modelMatrix * vec4(p_position, 1.0)).xyz;
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
}

void calculateScattering(inout vec4 finalColour)
{
	float fCameraHeight = distance(v3CameraPos, v3PlanetPos);
	float fScale = 1.0 / (fOuterRadius - fInnerRadius);
	float fFar, fNear, fTemp;
	bool inSpace = (fCameraHeight > fOuterRadius);
	
	vec3 v3Pos = mat3(modelMatrix) * p_position;
	vec3 v3Ray = v3Pos - v3CameraPos;
	fFar = length(v3Ray);
	v3Ray /= fFar;
	
	getSphereIntersection(v3CameraPos, v3Ray, fOuterRadius, fNear, fTemp);
	
	fNear = max(0.0, fNear);
	vec3 v3Start = v3CameraPos + v3Ray * fNear;
	float fRayLength = fFar - fNear;
	
	float fSampleLength = fRayLength / float(nSamples);
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;
	
	vec3 v3FrontColor = vec3(0.0);
	
	for(int i = 0; i < nSamples; i++)
	{
		float fDensity = density(distance(v3PlanetPos, v3SamplePoint));
			
		float fFarLight, fNearLight;
		getSphereIntersection(v3SamplePoint, v3LightDir, fOuterRadius, fNearLight, fFarLight);
		
		vec3 u = v3SamplePoint + v3LightDir * fFarLight;
		float n = (optic(v3Start, v3SamplePoint) + optic(v3SamplePoint, u)) * (PI * 4.0);
		
		v3FrontColor += fDensity * exp(-n * (fKr * v3InvWavelength + fKm));
		v3SamplePoint += v3SampleRay;
	}
	
	vec3 colour = fAtmosphericDensity * fRayLength * fScale * v3FrontColor * v3InvWavelength * fKr * fESun;
	vec4 scatteringColour = vec4(colour.rgb, 0.0);
	
	finalColour += scatteringColour;
	float brightness = dot(vec3(0.299,0.587,0.114), finalColour.rgb);
}

void main(void)
{
	vec4 finalColour = p_colour;
	
	calculateLighting(finalColour);
	
	if (hasAtmosphere)
	{
		calculateScattering(finalColour);
	}
	
	gl_FragDepth = log2(flogz) * (Fcoef / 2.0);
	colour = vec4(finalColour);
}