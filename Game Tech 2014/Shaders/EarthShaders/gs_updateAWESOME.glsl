#version 330 core
//gs_update
layout(points) in;
layout(points, max_vertices = 101) out;

#define PARTICLE_TYPE_LAUNCHER 0.0f                                                 
#define PARTICLE_TYPE_SHELL 1.0f                                                    
     
#define PARTICLE_AMOUNT 20.0	//the amount of generated particles each time

uniform float gDeltaTimeMillis;
uniform float gTime;

uniform sampler2D gRandomTexture1;
uniform sampler2D gRandomTexture2;


uniform float gLauncherLifetime;
uniform float gShellLifetime;
uniform float gSecondaryShellLifetime;

in particle{
	float Type0;
	vec3 Position0;
	vec3 Velocity0;
	float Age0;
	float Index0;
}IN[];


out particles{
	float type;
	vec3  pos;
	vec3  vel;
	float age;
	float   index;
}OUT;


vec3 GetRandomDir(float n)
{
	// float xx = 2.5*texture(gRandomTexture2, vec2(x, y)).x-1;
	// float yy = 0.3*texture(gRandomTexture2, vec2(x,y)).y;
	// vec3 Dir = vec3(xx, yy, 0);
	// return Dir;
	
	float xx = sin(36*n/3.14 );
	float yy = cos(n);
	vec3 Dir= vec3(xx,yy,0);
	return Dir;
	
}
vec3 GetRandomDir(float x, float y)
{
	float xx = 2*texture(gRandomTexture1, vec2(x, y)).x;
	float yy = 0.3*texture(gRandomTexture2, vec2(x,y)).z;
	vec3 Dir = vec3(xx, yy, 0);
	return Dir;

}

void main()
{
	float Age = IN[0].Age0 + gDeltaTimeMillis;
	
	if (IN[0].Type0 == PARTICLE_TYPE_LAUNCHER) {
		if (Age >= gLauncherLifetime) {
			for (int i = 0; i < PARTICLE_AMOUNT; i++){
				float random_index = gTime / 200.0 + i / 20.0;

				OUT.type = PARTICLE_TYPE_SHELL;

				//position level 1
				OUT.pos = IN[0].Position0;

				//velocity level 1
				vec3 Dir = GetRandomDir(random_index,  10000.0);
				//vec3 Dir = vec3(100, 10, 0);
				//vec3 Dir = GetRandomDir(i);
			
				OUT.vel = normalize(Dir);

				//age level 1
				OUT.age = 0.f;
				OUT.index = random_index;
				EmitVertex();
				EndPrimitive();
			}
			Age = 0.f;
		}

		//level 0 reset
		OUT.type = PARTICLE_TYPE_LAUNCHER;
		OUT.pos = IN[0].Position0;
		OUT.vel = IN[0].Velocity0;
		OUT.age = Age;
		OUT.index = IN[0].Index0;
		EmitVertex();
		EndPrimitive();
	}
	//it's not the level 0 particle
	else {
		float DeltaTimeSecs = gDeltaTimeMillis / 1000.0f;
		float t1 = IN[0].Age0 / 1000.0;
		float t2 = Age / 1000.0;
		vec3 DeltaP = DeltaTimeSecs * IN[0].Velocity0*5;

		if (IN[0].Type0 == PARTICLE_TYPE_SHELL)  {
			// it's level 1 particles
			if (Age < gShellLifetime) {
				OUT.type = PARTICLE_TYPE_SHELL;
				OUT.pos = IN[0].Position0 + DeltaP;
				
				//vec3 Dir = GetRandomDir(IN[0].Index0, gTime / 10000.0);
				vec3 Dir = GetRandomDir(IN[0].Index0);
				
				OUT.vel = normalize(Dir)  ;
				
				OUT.age = Age;
				OUT.index = IN[0].Index0;
				EmitVertex();
				EndPrimitive();
			}
			else {
				////it's time to launch new level 2 particles!!
				//for (int i = 0; i < 10; i++) {
				//	OUT.type = PARTICLE_TYPE_SECONDARY_SHELL;
				//	OUT.pos = IN[0].Position0;
				//	
				//	vec3 Dir  = GetRandomDir((gTime + 100*i) / 10000.0);
				//	OUT.vel = normalize(Dir);
				//	
				//	OUT.age = 0.0f;
				//	EmitVertex();
				//	EndPrimitive();
				//}
			}
		}
		else {
			////it's level 2 particles
			//if (Age < gSecondaryShellLifetime) {
			//	OUT.type = PARTICLE_TYPE_SECONDARY_SHELL;
			//	OUT.pos = IN[0].Position0 + DeltaP;
			//	//OUT.vel = IN[0].Velocity0 ;
			//	vec3 Dir = GetRandomDir(gTime / 10000.0);
			//	
			//	OUT.vel = normalize(Dir);
			//	OUT.age = Age;
			//	EmitVertex();
			//	EndPrimitive();
			//}
		}
	}
}
