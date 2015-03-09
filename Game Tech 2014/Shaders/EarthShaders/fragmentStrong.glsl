#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D flameTex;
in Vertex	{
//	vec4 colour;
	vec2 texCoord;
	float age2;
} IN;

out vec4 FragColor;

void main(void)	{

	vec4 flame_colour = vec4(texture(flameTex, vec2(1, IN.age2)).xyz, 1 - IN.age2);
	FragColor = flame_colour*texture(diffuseTex, IN.texCoord)*3;
//	FragColor = texture(diffuseTex, IN.texCoord);
}
