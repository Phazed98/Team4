#include "FireParticleSystem.h"

#define MAX_PARTICLES 20000
//#define PARTICLE_LIFETIME 10000.0f
#define SECOND_PARTICLE_LIFETIME 2000.0f

//#define PARTICLESIZE 20
//#define PARTICLE_LAUNCH_TIME 10.0f			// how often generate particles
#define LAUNCHER_AMOUNT 1


#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f

#define FIRE_SHADER_DIR "../../Shaders/fire-shader/"

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

FireParticleSystem::FireParticleSystem()
{

	
	m_currVB = 0;
	m_currTFB = 1;
	m_isFirst = true;
	m_time = 0;


	ZERO_MEM(m_transformFeedback);
	ZERO_MEM(m_particleBuffer);


	particle_texture = SOIL_load_OGL_texture(TEXTUREDIR"particle.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);

	random_texture1 = SOIL_load_OGL_texture(TEXTUREDIR"randomtexture1.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
	random_texture2 = SOIL_load_OGL_texture(TEXTUREDIR"randomtexture2.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);



	glBindTexture(GL_TEXTURE_2D, random_texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, random_texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

}


FireParticleSystem::~FireParticleSystem(){
	

	if (m_transformFeedback[0] != 0) {
		glDeleteTransformFeedbacks(2, m_transformFeedback);
	}

	if (m_particleBuffer[0] != 0) {
		glDeleteBuffers(2, m_particleBuffer);
	}
}

bool FireParticleSystem::InitParticleSystem(int shape_type, const Vector3& Pos, Camera* camera, Matrix4 pm)
{
	_shape = shape_type;

	this->camera = camera;

	project_matrix = pm;

	//this is basic fire shaders, you can change the geomertry shader to get different types
	//the fragment shader does not do anything
	switch (_shape){
	case 0:{
		particle_lifetime = 10000.f;
		render_particle_lifetime = particle_lifetime;
		particle_size = 20.f;
		first_particle_launch_time = 10.f;
		second_particle_lifetime = 2000.f;

		particleUpdateShader = new Shader(FIRE_SHADER_DIR"vs_update.glsl", FIRE_SHADER_DIR"fs_update.glsl", FIRE_SHADER_DIR"gs_update.glsl");
		flame_texture = SOIL_load_OGL_texture(TEXTUREDIR"flameColor.jpg",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
		break;
	}
	case 1:{
		particle_lifetime = 2000.f;
		particle_size = 25.f;
		first_particle_launch_time = 10.f;
		second_particle_lifetime = 2000.f;
		render_particle_lifetime = second_particle_lifetime;

		particleUpdateShader = new Shader(FIRE_SHADER_DIR"vs_update.glsl", FIRE_SHADER_DIR"fs_update.glsl", FIRE_SHADER_DIR"gs_explorsion.glsl");
		flame_texture = SOIL_load_OGL_texture(TEXTUREDIR"explosionColor.jpg",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
		break;
	}
	case 2:{
		particle_lifetime = 50.f;
		particle_size = 20;
		first_particle_launch_time = 50.f;
		second_particle_lifetime = 1000.f;
		render_particle_lifetime = second_particle_lifetime;
		particleUpdateShader = new Shader(FIRE_SHADER_DIR"vs_update.glsl", FIRE_SHADER_DIR"fs_update.glsl", FIRE_SHADER_DIR"gs_afterburner.glsl");

		//	flame_texture = yellow_afterburner_texture;
		afterburner_texture[0] = SOIL_load_OGL_texture(TEXTUREDIR"afterburner_yellow.png",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
		afterburner_texture[1] = SOIL_load_OGL_texture(TEXTUREDIR"afterburner_blue.png",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
		afterburner_index = 0;
		flame_texture = afterburner_texture[0];
		break;
	}
	}

	FireParticle Particles[MAX_PARTICLES];
	ZERO_MEM(Particles);

	for (int i = 0; i < LAUNCHER_AMOUNT; i++){
		//generate 4 particle launchers
		Particles[i].Type = PARTICLE_TYPE_LAUNCHER;
		Vector3 position_offset = Pos + Vector3(10*i,0,0);
		Particles[i].Pos = position_offset;
		Particles[i].Vel = Vector3(0.0f, 0.0001f, 0.0f);
		Particles[i].LifetimeMillis = 0.0f;
		Particles[i].index = -1;
	}

	//generate 2 transform feedback objects and each one has a buffer to do the ping-pong method
	glGenTransformFeedbacks(2, m_transformFeedback);
	glGenBuffers(2, m_particleBuffer);

	for (unsigned int i = 0; i < 2; i++) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
	}

	InitUpdateSystem();// initilize the particle update pipeline
	InitRenderSystem();// initilize the particle render pipeline

	
	return true;
}

bool FireParticleSystem::InitUpdateSystem(){
	//register the output stuffs to the transform feedback buffer
	//attention!! the value of strings should be exactly same as those in the shader(geomertry shader)
	const GLchar* Varyings[5];
	Varyings[0] = "particles.type";
	Varyings[1] = "particles.pos";
	Varyings[2] = "particles.vel";
	Varyings[3] = "particles.age";
	Varyings[4] = "particles.index";

	glTransformFeedbackVaryings(particleUpdateShader->GetProgram(), 5, Varyings, GL_INTERLEAVED_ATTRIBS);

	//then link the shaders
	if (!particleUpdateShader->LinkProgram()) {
		return false;
	}

	//get the input attributes location after we link it
	input_type	   = glGetAttribLocation(particleUpdateShader->GetProgram(), "p_type");
	input_position = glGetAttribLocation(particleUpdateShader->GetProgram(), "p_position");
	input_velocity = glGetAttribLocation(particleUpdateShader->GetProgram(), "p_velocity");
	input_lifetime = glGetAttribLocation(particleUpdateShader->GetProgram(), "p_age");
	input_index	   = glGetAttribLocation(particleUpdateShader->GetProgram(), "p_index");

	return true;
}

bool FireParticleSystem::InitRenderSystem(){
	particleRenderShader = new Shader(FIRE_SHADER_DIR"vertex.glsl", FIRE_SHADER_DIR"fragment.glsl", FIRE_SHADER_DIR"geometry.glsl");
	if (!particleRenderShader->LinkProgram()) {
		return false;
	}

	glUniform1i(glGetUniformLocation(particleRenderShader->GetProgram(), "diffuseTex"), 0);
	return true;
}

void FireParticleSystem::Render(int DeltaTimeMillis, const Matrix4& modelMatrix)
{
	m_time += DeltaTimeMillis;
//	cout << m_time << endl;
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	UpdateParticles(DeltaTimeMillis);
	
	

	RenderParticles(modelMatrix);

	m_currVB = m_currTFB;		//toggle input
	m_currTFB = (m_currTFB + 1) & 0x1;	//toggle output

	if (_shape == 2){
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {

			afterburner_index = (afterburner_index + 1) & 0x1;
			flame_texture = afterburner_texture[afterburner_index];
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {


			render_particle_lifetime = 300;
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {


			render_particle_lifetime = second_particle_lifetime;
		}
	}

	glEnable(GL_DEPTH_TEST);
	//reset the blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void FireParticleSystem::UpdateParticles(int DeltaTimeMillis)
{

	
	glUseProgram(particleUpdateShader->GetProgram());



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, random_texture1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, random_texture2);

	//set the total time pass
	glUniform1f(glGetUniformLocation(particleUpdateShader->GetProgram(), "gTime"), (float)m_time);
	//set the delta milliseconds
	glUniform1f(glGetUniformLocation(particleUpdateShader->GetProgram(), "gDeltaTimeMillis"), (float)DeltaTimeMillis);
	glUniform1f(glGetUniformLocation(particleUpdateShader->GetProgram(), "gLauncherLifetime"), first_particle_launch_time);
	glUniform1f(glGetUniformLocation(particleUpdateShader->GetProgram(), "gShellLifetime"), particle_lifetime);
	glUniform1f(glGetUniformLocation(particleUpdateShader->GetProgram(), "gSecondaryShellLifetime"), second_particle_lifetime);
	glUniform1i(glGetUniformLocation(particleUpdateShader->GetProgram(), "gRandomTexture1"), 0);
	glUniform1i(glGetUniformLocation(particleUpdateShader->GetProgram(), "gRandomTexture2"), 1);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currVB]);//bind input vertex buffer
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]);//bind output Tramsformfeedback object


	glVertexAttribPointer(input_type	, 1, GL_FLOAT, GL_FALSE, sizeof(FireParticle), 0);                  // type
	glVertexAttribPointer(input_position, 3, GL_FLOAT, GL_FALSE, sizeof(FireParticle), (const GLvoid*)4);   // position
	glVertexAttribPointer(input_velocity, 3, GL_FLOAT, GL_FALSE, sizeof(FireParticle), (const GLvoid*)16);  // velocity
	glVertexAttribPointer(input_lifetime, 1, GL_FLOAT, GL_FALSE, sizeof(FireParticle), (const GLvoid*)28);  // lifetime
	glVertexAttribPointer(input_index,	  1, GL_FLOAT, GL_FALSE, sizeof(FireParticle), (const GLvoid*)32);	// index

	glEnableVertexAttribArray(input_type);
	glEnableVertexAttribArray(input_position);
	glEnableVertexAttribArray(input_velocity);
	glEnableVertexAttribArray(input_lifetime);
	glEnableVertexAttribArray(input_index);
	
	
	GLuint query;
	glGenQueries(1, &query);
	
	glEnable(GL_RASTERIZER_DISCARD);
//	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);

//	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[m_currTFB]);
	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);

	glBeginTransformFeedback(GL_POINTS);

	if (m_isFirst) {
		glDrawArrays(GL_POINTS, 0, LAUNCHER_AMOUNT);

		m_isFirst = false;
	}
	else {
		glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currVB]);//attention!! this is the input buffer object
	}

	glEndTransformFeedback();

	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glDisable(GL_RASTERIZER_DISCARD);
//	glFlush();

	GLuint primitives;
	glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);
//	printf("%u primitives written!\n\n", primitives);
	
	glDisableVertexAttribArray(input_type);
	glDisableVertexAttribArray(input_position);
	glDisableVertexAttribArray(input_velocity);
	glDisableVertexAttribArray(input_lifetime);
	glDisableVertexAttribArray(input_index);


	/*FireParticle feedback[12];

	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);
	for (int i = 0; i < 12; i++){
		cout << "type:"  <<feedback[i].Type << endl;
		cout << "pos :" << feedback[i].Pos << endl;
		cout << "vel :"<< feedback[i].Vel << endl;
		cout << "life:" << feedback[i].LifetimeMillis << endl;
		cout << "index:" << feedback[i].index << endl;
		cout << "*********************************" << endl;
	}
	cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl;*/
	glUseProgram(0);

}


void FireParticleSystem::RenderParticles(const Matrix4& modelMatrix)
{
	Matrix4 model_matrix = modelMatrix*Matrix4::Scale(Vector3(5, 5, 5));
	Matrix4 view_matrix = camera->BuildViewMatrix();
	Matrix4 textureMatrix;

	glUseProgram(particleRenderShader->GetProgram());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, particle_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, flame_texture);

	glUniformMatrix4fv(glGetUniformLocation(particleRenderShader->GetProgram(), "modelMatrix"), 1, false, (float*)&model_matrix);
	glUniformMatrix4fv(glGetUniformLocation(particleRenderShader->GetProgram(), "viewMatrix"), 1, false, (float*)&view_matrix);
	glUniformMatrix4fv(glGetUniformLocation(particleRenderShader->GetProgram(), "projMatrix"), 1, false, (float*)&project_matrix);

	glUniform1f(glGetUniformLocation(particleRenderShader->GetProgram(), "MaxLifeTime"), render_particle_lifetime);

	glUniform1f(glGetUniformLocation(particleRenderShader->GetProgram(), "particleSize"), particle_size);
	glUniform1i(glGetUniformLocation(particleRenderShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(particleRenderShader->GetProgram(), "flameTex"), 1);
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);
//	glDisable(GL_RASTERIZER_DISCARD);
	

	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FireParticle), (const GLvoid*)4);  // position

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(FireParticle), (const GLvoid*)28);  // age

	glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0); //Remember to turn off our VAO ;)

	

	glUseProgram(0);
}

