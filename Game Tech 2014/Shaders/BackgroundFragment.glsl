#version 150 core

uniform sampler2D diffuseTex1;

uniform sampler2D diffuseTex2;

uniform float temp;

in Vertex	
{
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void)	
{
	//FragColor = texture(diffuseTex, IN.texCoord); //vec4(0,1.0,0.0,1.0);// 
	
	FragColor = temp*texture(diffuseTex1, IN.texCoord)+(1-temp)*texture(diffuseTex2, IN.texCoord);
}