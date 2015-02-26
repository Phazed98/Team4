#version 330 core

layout(points) in;
layout(points, max_vertices = 101) out;

#define PARTICLE_TYPE_LAUNCHER 0.0f                                                 
#define PARTICLE_TYPE_SHELL 1.0f                                                    
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f
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


vec3 GetRandomDir(float angle)
{
	float xx = cos(angle);
	float yy = sin(angle);
	vec3 Dir = vec3(xx, yy, 0);
	return Dir;
}

void main()
{
	float Age = IN[0].Age0 + gDeltaTimeMillis;
	
	if (IN[0].Type0 == PARTICLE_TYPE_LAUNCHER) {
		if (Age >= gLauncherLifetime) {
			for (int i = 0; i < PARTICLE_AMOUNT; i++){
			//	float random_index = gTime / 200.0 + i ;
				float random_index = i;
				OUT.type = PARTICLE_TYPE_SHELL;

				//position level 1
				OUT.pos = IN[0].Position0;

				//velocity level 1
				vec3 Dir = GetRandomDir(random_index);
				
			
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
		vec3 DeltaP = DeltaTimeSecs * IN[0].Velocity0*10;

		if (IN[0].Type0 == PARTICLE_TYPE_SHELL)  {
			// it's level 1 particles
			if (Age < gShellLifetime) {
				

				OUT.type = PARTICLE_TYPE_SHELL;
				OUT.pos = IN[0].Position0 + DeltaP;
				
				OUT.vel = IN[0].Velocity0;
			
				OUT.age = Age;
				OUT.index = IN[0].Index0;
				EmitVertex();
				EndPrimitive();
			}
			else {
				//this level 1 particle die

				OUT.type = PARTICLE_TYPE_SECONDARY_SHELL;
				OUT.pos = IN[0].Position0;


				OUT.vel = vec3(0, 0, 1);

				OUT.age = 0.0f;
				EmitVertex();
				EndPrimitive();
			}
			
		}
		else {
			//it's level 2 particles
			if (Age < gSecondaryShellLifetime) {
				OUT.type = PARTICLE_TYPE_SECONDARY_SHELL;
				OUT.pos = IN[0].Position0 + DeltaP*Age/100;
				
				
				OUT.vel = IN[0].Velocity0;
				OUT.age = Age;
				EmitVertex();
				EndPrimitive();
			}
		}
	}
}
