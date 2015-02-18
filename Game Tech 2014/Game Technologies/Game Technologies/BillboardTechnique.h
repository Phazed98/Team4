#pragma once

#include "Technique.h"
#include "../../nclgl/Shader.h"

class BillboardTechnique : public Technique{

public:
	BillboardTechnique();

	void SetVP(const Matrix4& VP);
	void SetCameraPosition(const Vector3& Pos);
	void SetColorTextureUnit(unsigned int TextureUnit);

	bool Init();
private:
	
	
	Shader* billShader;
	GLuint m_VPLocation;
	GLuint m_cameraPosLocation;
	GLuint m_colorMapLocation;

};