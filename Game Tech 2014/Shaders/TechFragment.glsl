#version 150 core

uniform sampler2D diffuseTex;
uniform int useTexture;

in Vertex	{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 OutFrag;

void main(void)	{
	OutFrag = IN.colour;
	if(useTexture > 0) {
		OutFrag	*= texture(diffuseTex, IN.texCoord);
	}
}