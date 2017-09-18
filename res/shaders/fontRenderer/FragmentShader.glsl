#version 400 core

in vec2 _position;
in vec2 _texture;
in vec4 _colour;

uniform sampler2D textureUnit;
uniform bool useLighting;

out vec4 outColour;

void main()
{
    outColour = vec4(_colour.rgb, texture2D(textureUnit, _texture).a * _colour.a);
}