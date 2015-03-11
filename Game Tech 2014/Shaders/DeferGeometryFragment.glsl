#version 330 core

in Vertex	{
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos ;

} IN;

layout (location = 0) out vec4 WorldPosOut; 
layout (location = 1) out vec4 DiffuseOut; 
layout (location = 2) out vec4 NormalOut; 
layout (location = 3) out vec4 TexCoordOut; 


uniform sampler2D diffuseTex;

void main(void)	{
	WorldPosOut = vec4(IN.worldPos,1);	
    DiffuseOut = texture(diffuseTex, IN.texCoord);	
    NormalOut = vec4(normalize(IN.normal),1);	
    TexCoordOut = vec4(IN.texCoord, 0,1); 
	
}