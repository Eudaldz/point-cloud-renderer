#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D colorSum;
uniform sampler2D colorWeight;

void main()
{ 
    vec4 c = texture(colorSum, TexCoords);
    float w = texture(colorWeight, TexCoords).r;
    FragColor.a = c.a;
    FragColor.rgb = mix(c.rgb/w, vec3(0,0,0), float(w==0));
    
}