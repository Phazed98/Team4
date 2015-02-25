# version 330 core

uniform sampler2D diffuseTex ;
uniform sampler2D depthTex ;
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
	/*
float ddd = texture2D(depthTex, texCoord).x;
	
	  FragColor = vec4(ddd,ddd,ddd,1);
*/
float zOverW = texture(depthTex, texCoord).r;
	//H is the viewport position at this pixel in the range -1 to 1.

   vec4 H = vec4((1 - texCoord.x) * 2 - 1, (1 - texCoord.y) * 2 - 1, zOverW, 1);  
//	vec4 H = vec4(texCoord.x * 2 - 1, texCoord.y * 2 - 1, zOverW, 1);
	//Transform by the view-projection inverse.  
   vec4 D = inversePVMatrix * H;
	
	//Divide by w to get the world position.  
   vec4 worldPos = D / D.w;  

	// Current viewport position  
   vec4 currentPos = H;  
	
	// Use the world position, and transform by the previous view-  
   	// projection matrix.    
   vec4 previousPos = previousPVMatrix * worldPos ;  
	
	// Convert to nonhomogeneous points [-1,1] by dividing by w.  
   previousPos /= previousPos.w;  

	// Use this frame's position and last frame's to compute the pixel  
	// velocity.  
   vec2 velocity = ((currentPos - previousPos)/2.f).xy;  

	// Get the initial color at this pixel.  
   vec4 color = texture(diffuseTex, texCoord);
   texCoord += velocity;  

   for(int i = 1; i < 5; ++i, texCoord += velocity){  
  	// Sample the color buffer along the velocity vector.  
		vec4 currentColor = texture(diffuseTex, texCoord);
  	// Add the current color to our color sum.  
		color += currentColor;  
	}

	// Average all of the samples to get the final blur color.  
   vec4 outputColor = FragColor = color / 5; 

   float fFogCoord = abs(LinearizeDepth(zOverW) );
   float fResult = exp(-pow(fFogCoord, 2.0));

   fResult = 1.0-clamp(fResult, 0.0, 1.0);
   FragColor = mix(outputColor, vec4(0.7,0.7,0.7,1), fResult);



//  FragColor  = vec4(ddd,ddd ,ddd ,1);
}