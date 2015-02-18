#pragma once

#include "../../nclgl/Vector3.h"
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Matrix4.h"
#include "../../nclgl/Shader.h"
#include "../../nclgl/Mesh.h"
#include "../../nclgl/Camera.h"
#include "BillboardTechnique.h"
class BillboardList
{
public :
	BillboardList();
	~BillboardList();

	//bool Init(const std::string& TexFilename);
	void Render(const Matrix4& VP, const Vector3& CameraPos);

private:
	void CreatePositionBuffer();

	GLuint m_VB;
	GLuint BiillboardTexture;
	BillboardTechnique m_technique;
};