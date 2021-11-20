#version 330 core
in vec2 TexCoords;

uniform sampler2D Color;

void main()
{ 
    float a = texture(Color, TexCoords).a;
    if(a > 0)discard;
}