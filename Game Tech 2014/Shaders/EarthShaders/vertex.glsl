#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float MaxLifeTime;

in  vec3 position;
in float age;

out float age1;

void main(void)	{
	mat4 mvp 		= projMatrix * viewMatrix * modelMatrix;
	gl_Position		= mvp * vec4(position, 1.0);
	age1 = age / MaxLifeTime*0.7;	 //cut it to (0,1)
}