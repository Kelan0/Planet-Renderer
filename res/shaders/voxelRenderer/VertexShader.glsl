#version 400 core

in vec3 position;
in vec3 normal;
in vec2 texture;
in vec4 colour;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float Fcoef;
uniform float near;
uniform float far;
uniform float scale;

out vec3 p_position;
out vec3 p_normal;
out vec2 p_texture;
out vec4 p_colour;
out float flogz;


void main(void)
{
	 p_position = position;
	 p_normal = normal;
	 p_texture = texture;
	 p_colour = colour;

	//MVP = projectionMatrix * viewMatrix * modelMatrix
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
	gl_Position.z = log2(max(near, 1.0 + gl_Position.w)) * Fcoef - 1.0;
	flogz = 1.0 + gl_Position.w;
}