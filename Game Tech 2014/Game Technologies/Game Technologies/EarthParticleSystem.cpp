#include "EarthParticleSystem.h"

#define MAX_PARTICLES 20000
#define SECOND_PARTICLE_LIFETIME 2000.0f
#define PARTICLE_LAUNCH_TIME 10.0F
#define LAUNCHER_AMOUNT 1

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f

#define EARTH_SHADER_DIR "../../Shaders/EarthShaders/"

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define SAFE_DELETE(p) if (p){ delete p; p = NULL;}


EarthParticleSystem::EarthParticleSystem(){
	
	m_currVB=0;
	m_currTFB = 1;
	m_isFirst+true;
	m_time = 0;

	ZERO_MEM(m_transformFeedback);
	ZERO_MEM(m_particleBuffer);

	particle_texture = SOIL_load_OGL_texture(TEXTUREDIR"particle.tga",SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);

	random_texture1 = SOIL_load_OGL_texture(TEXTUREDIR"randomtexture1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
	
	random_texture2 = SOIL_load_OGL_texture(TEXTUREDIR"randomtexture2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);


	glBindTexture(GL_TEXTURE_2D, random_texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, random_texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

EarthParticleSystem::~EarthParticleSystem(){
	
	if (m_transformFeedback[0] != 0) {
		glDeleteTransformFeedbacks(2, m_transformFeedback);
	}

	if (m_particleBuffer[0] != 0) {
		glDeleteBuffers(2, m_particleBuffer);
	}
}

bool EarthParticleSystem::InitParticleSystem(int shape_type, const Vector3& Pos){

	_shape = shape_type;

	switch (_shape){
	case 0:{
		particle_lifetime = 10000.f;
		particle_size = 40;
		particleUpdateShader = new Shader(EARTH_SHADER_DIR"vs_update.glsl", EARTH_SHADER_DIR"fs_update.glsl", EARTH_SHADER_DIR"gs_whirl.glsl");
		flame_texture = SOIL_load_OGL_texture(TEXTUREDIR"TornadoColor2.jpg",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
		break;

	}
	case 1:{
		particle_lifetime = 10000.f;
		particle_size = 60;
		particleUpdateShader = new Shader(EARTH_SHADER_DIR"vs_update.glsl", EARTH_SHADER_DIR"fs_update.glsl", EARTH_SHADER_DIR"gs_update.glsl");
		flame_texture = SOIL_load_OGL_texture(TEXTUREDIR"dust2.jpg",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
		break;
	}
	case 2:{
		particle_lifetime = 2000.0f;
		particle_size = 30;
		first_particle_launch_time = 10000.f;
		second_particle_lifetime = 1000.f;
		render_particle_lifetime = second_particle_lifetime;
		particleUpdateShader = new Shader(EARTH_SHADER_DIR"vs_update.glsl", EARTH_SHADER_DIR"fs_update.glsl", EARTH_SHADER_DIR"gs_afterburner2.glsl");
		flame_texture = SOIL_load_OGL_texture(TEXTUREDIR"waterImg2.jpg",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
		break;
	}
	}

	EarthParticle Particles[MAX_PARTICLES];
	ZERO_MEM(Particles);

	for (int i = 0; i < LAUNCHER_AMOUNT; i++){
		Particles[i].type = PARTICLE_TYPE_LAUNCHER ;
		Vector3 position_offset = Pos + Vector3(10*i, 0, 0);
		Particles[i].Pos = position_offset;
		Particles[i].Vel = Vector3(0.0f, 0.01f, 0.0f);
		Particles[i].Lifetime = 0.0f;
		Particles[i].index = -1;
	}
	
	glGenTransformFeedbacks(2, m_transformFeedback);
	glGenBuffers(2, m_particleBuffer); 

	for (unsigned int i = 0; i < 2; i++){
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
	}

	InitUpdateSystem();
	InitRenderSystem(_shape);

	return true;
}

bool EarthParticleSystem::InitUpdateSystem(){
	const GLchar* Varyings[5];
	Varyings[0] = "particles.type";
	Varyings[1] = "particles.pos";
	Varyings[2] = "particles.vel";
	Varyings[3] = "particles.age";
	Varyings[4] = "particles.index";

	glTransformFeedbackVaryings(particleUpdateShader->GetProgram(), 5, Varyings, GL_INTERLEAVED_ATTRIBS);

	if (!particleUpdateShader->LinkProgram()){
		return false;
	}

	//get The input attributes location after linking it

	input_type = glGetAttribLocation(particleUpdateShader->GetProgram(), "p_type");
	input_position = glGetAttribLocation(particleUpdateShader->GetProgram(), "p_position");
	input_velocity = glGetAttribLocation(particleUpdateShader->GetProgram(), "p_velocity");
	input_lifetime = glGetAttribLocation(particleUpdateShader->GetProgram(), "p_age");
	input_index = glGetAttribLocation(particleUpdateShader->GetProgram(), "p_index");

	return true;

	
}

bool EarthParticleSystem::InitRenderSystem(int shape_type){

	switch (_shape){
	case 0:{
	
		particleRenderShader = new Shader(EARTH_SHADER_DIR"vertex.glsl", EARTH_SHADER_DIR"fragment.glsl", EARTH_SHADER_DIR"geometry.glsl");
		break;
	}
	case 1:{
		particleRenderShader = new Shader(EARTH_SHADER_DIR"vertexStrong.glsl", EARTH_SHADER_DIR"fragmentStrong.glsl", EARTH_SHADER_DIR"geometry.glsl");
		break;
	}
	case 2:{
		particleRenderShader = new Shader(EARTH_SHADER_DIR"vertex.glsl", EARTH_SHADER_DIR"fragmentMedium.glsl", EARTH_SHADER_DIR"geometry.glsl");
		break;
	}
	}
	if (!particleRenderShader->LinkProgram()){
		return false;
	}

	glUniform1i(glGetUniformLocation(particleRenderShader->GetProgram(), "diffuseTex"), 0);

	return true;
}

void EarthParticleSystem::Render(float DeltaTime, const Matrix4& modelMatrix, const Matrix4& projMatrix , const Matrix4& viewMatrix ){

	project_matrix = projMatrix;
	view_matrix = viewMatrix;
	m_time += DeltaTime;
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	UpdateParticles(DeltaTime);

	RenderParticles(modelMatrix);

	m_currVB = m_currTFB; //toggle input
	m_currTFB = (m_currTFB +1) & 0x1 ; //toggle output

	render_particle_lifetime = second_particle_lifetime;

}

void EarthParticleSystem::UpdateParticles(int DeltaTime){
	
	glUseProgram(particleUpdateShader->GetProgram());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, random_texture1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, random_texture2);

	//set the total time pass
	glUniform1f(glGetUniformLocation(particleUpdateShader->GetProgram(), "gTime"), (float)m_time);
	//set the delta milliseconds
	glUniform1f(glGetUniformLocation(particleUpdateShader->GetProgram(), "gDeltaTimeMillis"), (float)DeltaTime);
	glUniform1f(glGetUniformLocation(particleUpdateShader->GetProgram(), "gLauncherLifetime"), PARTICLE_LAUNCH_TIME);
	glUniform1f(glGetUniformLocation(particleUpdateShader->GetProgram(), "gShellLifetime"), particle_lifetime);
	glUniform1f(glGetUniformLocation(particleUpdateShader->GetProgram(), "gSecondaryShellLifetime"), SECOND_PARTICLE_LIFETIME);
	glUniform1i(glGetUniformLocation(particleUpdateShader->GetProgram(), "gRandomTexture1"), 0);
	glUniform1i(glGetUniformLocation(particleUpdateShader->GetProgram(), "gRandomTexture2"), 1);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currVB]);//bind input vertex buffer
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]);//bind output Tramsformfeedback object

	//Set up the vertex attributes of the particles in the vertyex buffer

	glVertexAttribPointer(input_type, 1, GL_FLOAT, GL_FALSE, sizeof(EarthParticle), 0);                  // type
	glVertexAttribPointer(input_position, 3, GL_FLOAT, GL_FALSE, sizeof(EarthParticle), (const GLvoid*)4);   // position
	glVertexAttribPointer(input_velocity, 3, GL_FLOAT, GL_FALSE, sizeof(EarthParticle), (const GLvoid*)16);  // velocity
	glVertexAttribPointer(input_lifetime, 1, GL_FLOAT, GL_FALSE, sizeof(EarthParticle), (const GLvoid*)28);  // lifetime
	glVertexAttribPointer(input_index, 1, GL_FLOAT, GL_FALSE, sizeof(EarthParticle), (const GLvoid*)32);	// index

	glEnableVertexAttribArray(input_type);
	glEnableVertexAttribArray(input_position);
	glEnableVertexAttribArray(input_velocity);
	glEnableVertexAttribArray(input_lifetime);
	glEnableVertexAttribArray(input_index);

	GLuint query;
	glGenQueries(1, &query);

	glEnable(GL_RASTERIZER_DISCARD); //Discard all the primitives before they reach rasterizer

	//	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);

	//	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[m_currTFB]);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);

	glBeginTransformFeedback(GL_POINTS); // All the draw calls after this until the glEndTransformFeedback() have their output redirected to the transform feedback buffer.

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


void EarthParticleSystem::RenderParticles(const Matrix4& modelMatrix)
{
	Matrix4 model_matrix ;
	switch (_shape){
	case 1:{
	
		model_matrix = modelMatrix*Matrix4::Scale(Vector3(60, 60, 40));
		break;
	}
	case 0:{
		model_matrix = modelMatrix*Matrix4::Scale(Vector3(5, 5, 5));
		break;
	}
	case 2:{
		model_matrix = modelMatrix*Matrix4::Scale(Vector3(5, 5, 5));
		break;
	}
	}

	//cout << model_matrix.values[12] << " " << model_matrix.values[13] << " " << model_matrix.values[14] << endl;
	Matrix4 textureMatrix;

	glUseProgram(particleRenderShader->GetProgram());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, particle_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, flame_texture);

	glUniformMatrix4fv(glGetUniformLocation(particleRenderShader->GetProgram(), "modelMatrix"), 1, false, (float*)&model_matrix);
	glUniformMatrix4fv(glGetUniformLocation(particleRenderShader->GetProgram(), "viewMatrix"), 1, false, (float*)&view_matrix);
	glUniformMatrix4fv(glGetUniformLocation(particleRenderShader->GetProgram(), "projMatrix"), 1, false, (float*)&project_matrix);
	if (_shape == 0)
		glUniform1f(glGetUniformLocation(particleRenderShader->GetProgram(), "MaxLifeTime"), particle_lifetime);
	if (_shape == 1)
		glUniform1f(glGetUniformLocation(particleRenderShader->GetProgram(), "MaxLifeTime"), SECOND_PARTICLE_LIFETIME);
	glUniform1f(glGetUniformLocation(particleRenderShader->GetProgram(), "particleSize"), particle_size);
	glUniform1i(glGetUniformLocation(particleRenderShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(particleRenderShader->GetProgram(), "flameTex"), 1);
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);
	//	glDisable(GL_RASTERIZER_DISCARD);



	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(EarthParticle), (const GLvoid*)4);  // position

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(EarthParticle), (const GLvoid*)28);  // age

	glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0); //Remember to turn off our VAO ;)

	glUseProgram(0);
}

