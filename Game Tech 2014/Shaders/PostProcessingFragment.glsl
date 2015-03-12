#version 150 core

uniform sampler2D diffuseTex;


uniform float DistortionAmount;
uniform float times;
uniform int   current_plane;

in Vertex	{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 FragColor;

void main(void)	{
	if(current_plane == 1){
		float TimeInRadians_x = times*2;
		float TimeInRadians_y = times*2;
		float offset_x = sin(TimeInRadians_x+IN.texCoord.x*4*3.14)*DistortionAmount;
		float offset_y = cos(TimeInRadians_y+IN.texCoord.y*5*3.14)*DistortionAmount;

		vec2 texCoord = vec2(IN.texCoord.x+offset_x,IN.texCoord.y+offset_y);
		FragColor = texture(diffuseTex, texCoord);

	}
	else
		FragColor	= texture(diffuseTex, IN.texCoord);
	
}