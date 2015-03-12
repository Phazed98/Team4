# version 330 core

uniform sampler2D diffuseTex ;
uniform sampler2D depthTex ;
uniform sampler2D particleTex ;
uniform sampler2D backgroundTex;

uniform mat4 previousPVMatrix;

uniform mat4 inversePVMatrix;

in Vertex {
vec2 texCoord ;
}IN;

out vec4 FragColor;

float LinearizeDepth(float ZoverW){
	float f=10000.0;
	float n = 1;
	float z = (2 * n) / (f + n - ZoverW * (f - n));
	return z;
}


void main ( void ) {


	//Get the depth buffer value at this pixel.
	vec2 texCoord = IN.texCoord;

	//Get the current particle texture
	vec4 particle_color = texture(particleTex, texCoord);
	//Get the current background texture
	vec4 background_colour = texture(backgroundTex, texCoord);
	
	float zOverW = texture(depthTex, texCoord).r;
	
	//H is the viewport position at this pixel in the range -1 to 1.
	vec4 H = vec4(texCoord.x * 2 - 1, (1 - texCoord.y) * 2 - 1, zOverW, 1);  

	//Transform by the view-projection inverse.  
	vec4 D = inversePVMatrix * H;

	//Divide by w to get the world position.  
	vec4 worldPos = D / D.w;  

	// Current viewport position  
	vec4 currentPos = H;  

	// Use the world position, and transform by the previous view-  
	// projection matrix. 
//	vec4 previousWorldPos = worldPos;
//	previousWorldPos.z = previousWorldPos.z-10000;
	vec4 previousPos = previousPVMatrix * worldPos ;  

	// Convert to nonhomogeneous points [-1,1] by dividing by w.  
	previousPos /= previousPos.w;  
//	previousPos.z = currentPos.z-0.1;
	// Use this frame's position and last frame's to compute the pixel velocity.  
	vec2 velocity = ((previousPos-currentPos)/1000.f).xy; 
//	vec2 velocity = ((currentPos-previousPos)).xy; 
	// Get the initial color at this pixel.  
	vec4 motion_color = texture(diffuseTex, texCoord);
	  
//if(texCoord.x<0.2||texCoord.x>0.8||texCoord.y<0.2||texCoord.y>0.6){
	//do motion blur
	texCoord += velocity;
	int count = 1;
	for(int i = 1; i < 5; ++i, texCoord += velocity){  
		// Sample the color buffer along the velocity vector.  
		vec4 currentColor = texture(diffuseTex, texCoord);
		if(currentColor.x!=0){
			//if the color is not black
			// Add the current color to our color sum.  
			motion_color += currentColor;
			count++;
		}
		
	}

	// Average all of the samples to get the final blur color.  
	motion_color = motion_color / count; 
//}
	
	float fFogCoord = abs(LinearizeDepth(zOverW) );
	float fResult = exp(-pow(fFogCoord*1.2, 2.0));

	fResult = 1.0-clamp(fResult, 0.0, 1.0);
	FragColor = mix(motion_color, vec4(0.5,0.5,0.5,1), fResult);


//	FragColor += motion_color;
	
	if(LinearizeDepth(zOverW)>0.9)
		
		FragColor = background_colour;
	
	FragColor += particle_color;
//	FragColor = motion_color;
//	FragColor = texture(diffuseTex, IN.texCoord);
		
//	FragColor = vec4(velocity,0,1);
}