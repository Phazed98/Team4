#pragma once

#include "../../nclgl/Vector3.h"
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Matrix4.h"
#include "../../nclgl/Shader.h"
#include "../../nclgl/Mesh.h"
#include "../../nclgl/Camera.h"

struct EarthParticle{
	float type;
	Vector3 Pos;
	Vector3 Vel;
	float Lifetime;
	float index;
};

class EarthParticleSystem{
public:
	EarthParticleSystem();
	~EarthParticleSystem();

	bool InitParticleSystem(int shape_type, const Vector3& Pos);

	void Render(int DeltaTime, const Matrix4& viewMatrix, const Matrix4& projMatrix, const Vector3& CameraPos = Vector3());

private:

	void UpdateParticles(int DeltaTime);
	void RenderParticles(const Matrix4& viewMatrix, const Matrix4& projMatrix);
	bool InitUpdateSystem();
	bool InitRenderSystem();

	bool m_isFirst;
	unsigned int m_currVB;
	unsigned int m_currTFB;
	GLuint m_particleBuffer[2];
	GLuint m_transformFeedback[2];

	int m_time;
	GLuint particle_texture;
	GLuint flame_texture;
	GLuint random_texture1;
	GLuint random_texture2;

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

	int particle_lifetime;
	int particle_size;

	int _shape;
};
