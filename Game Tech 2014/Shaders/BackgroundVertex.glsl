#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec2 texCoord;

out Vertex 
{
	vec2 texCoord;
} OUT;


 void main ( void ) {
	 vec3 tempPos = position - vec3 (0 ,0 ,1);

	 gl_Position = projMatrix * vec4 ( tempPos , 1.0);
	 OUT.texCoord = texCoord;
 //gl_Position =  vec4 ( tempPos , 1.0);
 }