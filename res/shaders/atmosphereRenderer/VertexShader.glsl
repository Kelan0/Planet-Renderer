#version 400 core

in vec3 position;
in vec3 normal;
in vec2 texture;
in vec4 colour;

uniform mat4  modelMatrix;
uniform mat4  viewMatrix;
uniform mat4  projectionMatrix;
uniform float near;
uniform float Fcoef;

out float flogz;
out vec3 p_position;

void main(void)
{
	p_position = position;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
	gl_Position.z = log2(max(near, 1.0 + gl_Position.w)) * Fcoef - 1.0;
	
	flogz = 1.0 + gl_Position.w;
}