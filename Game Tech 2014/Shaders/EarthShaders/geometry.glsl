#version 330 core
 

uniform float particleSize;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;


in float age1[];


out Vertex {
//	vec4 colour;
	vec2 texCoord;
	float age2;
} OUT;
 
void main() {	

	float particle_scale = particleSize*(sin(0.5+age1[0]*3.14));

	for(int i = 0; i < gl_in.length(); ++i) {
		//top right
		gl_Position = gl_in[ i ].gl_Position;
		gl_Position.x += particle_scale;
		gl_Position.y += particle_scale;
		OUT.texCoord = vec2(1,0);
		OUT.age2 = age1[0];
	
		EmitVertex();

		//Top Left
		gl_Position = gl_in[ i ].gl_Position;
		gl_Position.x -= particle_scale;
		gl_Position.y += particle_scale;
		OUT.texCoord = vec2(0,0);
		OUT.age2 = age1[0];
		EmitVertex();

		//bottom right
		gl_Position = gl_in[ i ].gl_Position;
		gl_Position.x += particle_scale;
		gl_Position.y -= particle_scale;
		OUT.texCoord = vec2(1,1);
		OUT.age2 = age1[0];
		EmitVertex();

		//bottom Left
		gl_Position = gl_in[ i ].gl_Position;
		gl_Position.x -= particle_scale;
		gl_Position.y -= particle_scale;
		OUT.texCoord = vec2(0,1);
		OUT.age2 = age1[0];
		EmitVertex();

		EndPrimitive();
	}
}
