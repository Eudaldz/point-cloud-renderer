#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D ColorSum;
uniform sampler2D AlphaWeightSum;

void main()
{ 
    vec3 c = texture(ColorSum, TexCoords).rgb;
    vec2 alpha_weight = texture(AlphaWeightSum, TexCoords).rg;
    if(alpha_weight.g == 0 || alpha_weight.r == 0)discard;
    float factor = alpha_weight.g <= 1.0f ? 1.0f : 1.0f/alpha_weight.g;
    FragColor.rgb = c * factor;
    FragColor.a = alpha_weight.r * factor;
}