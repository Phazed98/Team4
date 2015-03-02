#version 150 core

uniform sampler2D diffuseTex;

in Vertex	
{
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void)	
{
	FragColor = texture(diffuseTex, IN.texCoord); //vec4(0,1.0,0.0,1.0);// 
}