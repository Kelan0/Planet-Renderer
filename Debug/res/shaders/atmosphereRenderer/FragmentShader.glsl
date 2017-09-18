#version 400 core

invariant in vec3 vertexToCamera;

in vec4 dgl_SecondaryColor;
in vec4 dgl_Color;
in float flogz;

uniform vec3 lightPos;
uniform float g;
uniform float Fcoef;

out vec4 colour;

void main (void)
{
	float g2 = g * g;
	float fCos = dot(lightPos, vertexToCamera) / length(vertexToCamera);
	float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos * fCos) / pow(1.0 + g2 - 2.0 * g * fCos, 1.5);
	
	gl_FragDepth = log2(flogz) * (Fcoef / 2.0);
	
	colour = dgl_Color + fMiePhase * dgl_SecondaryColor;
	colour.a = colour.b;
}