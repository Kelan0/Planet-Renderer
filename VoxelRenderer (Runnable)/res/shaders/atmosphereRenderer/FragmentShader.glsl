#version 400 core

const int nSamples = 16;
const float PI = 3.14159265358979323846264338327950288;

in float flogz;
//in vec3 c0;
//in vec3 c1;
//in vec3 t0;
in vec3 p_position;

uniform mat4  modelMatrix;
uniform mat4  viewMatrix;
uniform mat4  projectionMatrix;
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
uniform float Fcoef;

out vec4 outColour;

float getMiePhase(float fCos, float fCos2, float g, float g2)
{
	return 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos2) / pow(1.0 + g2 - 2.0 * g * fCos, 1.5);
}

float getRayleighPhase(float fCos2)
{
	return 1.0;//0.75 * (1.0 + fCos2);
}

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
	
	return max(fDensity, 0.0);
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

void checkValidFragment(vec3 v3Ray) //check if this fragment is behind the horizon, or above the atmosphere.
{
	float fNear = 0.0;
	float fFar = 0.0;
	if (getSphereIntersection(v3CameraPos, v3Ray, fInnerRadius, fNear, fFar) && fFar > abs(fNear))
	{
		//if the ray intersects the inner radius (the planet), and the near value is on the other side of the planet, discard this fragment.
		discard;
	}
}

void main(void)
{
	float fCameraHeight = distance(v3CameraPos, v3PlanetPos);
	float fNear, fFar;
	bool inSpace = (fCameraHeight> fOuterRadius);
	
	vec3 v3Pos = mat3(modelMatrix) * p_position;
	vec3 v3Ray = normalize(v3Pos - v3CameraPos);
	
	checkValidFragment(v3Ray);
	getSphereIntersection(v3CameraPos, v3Ray, fOuterRadius, fNear, fFar);
	
	vec3 v3Start = (inSpace ? v3CameraPos + v3Ray * fNear : v3CameraPos);
	float fRayLength = inSpace ? fFar - fNear : fFar;
	
	float fHeight, fDensity;
	
	float fSampleLength = fRayLength / float(nSamples);
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;
	
	vec3 v3FrontColor = vec3(0.0);
	
	for(int i = 0; i < nSamples; i++)
	{
		fHeight = distance(v3PlanetPos, v3SamplePoint);
		fDensity = density(fHeight);
		
		float fFarLight = 0.0;
		float fNearLight = 0.0;
		getSphereIntersection(v3SamplePoint, v3LightDir, fOuterRadius, fNearLight, fFarLight);
		
		vec3 u = v3SamplePoint + v3LightDir * fFarLight;
		float n = (optic(v3Start, v3SamplePoint) + optic(v3SamplePoint, u)) * (PI * 4.0);
		
		v3FrontColor += fDensity * exp(-n * (fKr * v3InvWavelength + fKm));
		v3SamplePoint += v3SampleRay;
	}
	
	vec3 c0 = v3FrontColor * (v3InvWavelength * fKr * fESun);
	vec3 c1 = v3FrontColor * fKm * fESun;
	vec3 t0 = v3CameraPos - v3Pos;
	
	float g2 = g * g;
	float fCos = dot(v3LightDir, t0) / length(t0);
	float fCos2 = fCos * fCos;
	vec3 colour = getRayleighPhase(fCos2) * c0 + getMiePhase(fCos, fCos2, g, g2) * c1;

	outColour = vec4(colour.rgb, colour.r);
	gl_FragDepth = log2(flogz) * (Fcoef / 2.0);
}