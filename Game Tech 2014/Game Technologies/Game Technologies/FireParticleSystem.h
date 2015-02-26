
//http://ogldev.atspace.co.uk/www/tutorial28/tutorial28.html

#pragma once



#include "./nclgl/Vector3.h"
#include "./NCLGL/OGLRenderer.h"
#include "./nclgl/Matrix4.h"
#include "./nclgl/Shader.h"
#include "./nclgl/Mesh.h"
#include "./nclgl/camera.h"


struct FireParticle
{
	float Type;
	Vector3 Pos;
	Vector3 Vel;
	float LifetimeMillis;
	float index;
};



class FireParticleSystem //:public Mesh
{
public:
	FireParticleSystem();

	~FireParticleSystem();

	//set the type we want to render, then get its postion
	bool InitParticleSystem(int shape_type, const Vector3& Pos);

	//get everything we need then render it
	void Render(float DeltaTimeMillis, const Matrix4& modelMatrix = Matrix4(), 
		const Matrix4& projMatrix = Matrix4(), const Matrix4& viewMatrix = Matrix4());

private:
	//go through the transform feedback system to update each particles' position...
	void UpdateParticles(int DeltaTimeMillis);

	//render particles acconding to the modelmatrix
	void RenderParticles(const Matrix4& modelMatrix);
	
	//initilize the update system and render system
	bool InitUpdateSystem();
	bool InitRenderSystem();

	//for the first time render 1 particle to system
	bool m_isFirst;

	unsigned int m_currVB;
	unsigned int m_currTFB;
	GLuint m_particleBuffer[2];
	GLuint m_transformFeedback[2];

	float m_time;
	GLuint particle_texture;
	GLuint flame_texture;
	GLuint random_texture1;
	GLuint random_texture2;
	GLuint yellow_afterburner_texture;
	GLuint blue_afterburner_texture;
	GLuint afterburner_texture[2];

	Shader* particleUpdateShader;
	Shader* particleRenderShader;


	GLuint m_deltaTimeMillisLocation;
	GLuint m_randomTextureLocation;
	GLuint m_timeLocation;
	GLuint m_launcherLifetimeLocation;
	GLuint m_shellLifetimeLocation;
	GLuint m_secondaryShellLifetimeLocation;

	GLint input_type;
	GLint input_position;
	GLint input_velocity;
	GLint input_lifetime;
	GLint input_index;

	float particle_lifetime;
	float particle_size;
	float first_particle_launch_time;
	float second_particle_lifetime;
	float render_particle_lifetime;
	int _shape;

	int afterburner_index;
	Camera* camera;
	Matrix4 project_matrix;
	Matrix4 view_matrix;
};