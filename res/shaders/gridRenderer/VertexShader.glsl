#version 400 core

in vec3 position;
in vec3 normal;
in vec2 texture;
in vec4 colour;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float scale;
uniform vec4 bodies[10];

out vec3 p_position;
out vec3 p_normal;
out vec2 p_texture;
out vec4 p_colour;

void main(void)
{
	vec4 vertexPosition = modelMatrix * vec4(position, 1.0);
	
	for (int i = 0; i < 10; i++)
	{
		vec4 body = bodies[i];
		
		vec4 bodyPosition = vec4(body.x, 0.0, body.y, 1.0);
		float gravity = body.z;
		float radius = body.w * scale;
	
		vertexPosition.y -= (gravity * scale) / length(bodyPosition.xyz - vertexPosition.xyz);
	}
	
	p_position = position;
	p_normal = normal;
	p_texture = texture;
	p_colour = colour;
	
	gl_Position = projectionMatrix * viewMatrix * vertexPosition;
}