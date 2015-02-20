#version 150 core

uniform sampler2D diffuseTex;

in Vertex	
{
	vec2 texCoord;
} IN;

out vec4 gl_FragColor;

void main(void)	
{
	vec2 correctedTex = vec2(IN.texCoord.x, IN.texCoord.y);
	gl_FragColor = texture(diffuseTex, correctedTex);//IN.texCoord); //vec4(0,1.0,0.0,1.0);// 
}