#version 150
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 OutFrag;

void main(void){
	OutFrag = texture(diffuseTex, IN.texCoord);
}