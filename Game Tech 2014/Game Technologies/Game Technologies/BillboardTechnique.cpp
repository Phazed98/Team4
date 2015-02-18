#include "BillboardTechnique.h"

BillboardTechnique::BillboardTechnique(){

	billShader = new Shader("../../billboardVS.glsl", "../../billboardFS.glsl", "../../billboardGS.glsl");

}

bool BillboardTechnique::Init(){
	
	if (!Technique::Init()){
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "billboardVS.glsl")){
		return false;
	}
	if (!AddShader(GL_GEOMETRY_SHADER, "billboardGS.glsl")){
		return false;
	}
	if (!AddShader(GL_FRAGMENT_SHADER, "billboardFS.glsl")){
		return false;
	}

	if (!Finalize()){
		return false;
	}

	//m_VPLocation = GetUniformLocation("gVP");
	
	glUniform1f(glGetUniformLocation(billShader->GetProgram(), "gVP"), m_VPLocation);
	glUniform1f(glGetUniformLocation(billShader->GetProgram(), "gCameraPos"), m_cameraPosLocation);
	glUniform1f(glGetUniformLocation(billShader->GetProgram(), "gColorMap"), m_colorMapLocation);
	
}

void BillboardTechnique::SetVP(const Matrix4& viewMatrix){

	
	glUniformMatrix4fv(glGetUniformLocation(billShader->GetProgram(), "gVP"), 1, false, (float*)&viewMatrix);

}

void BillboardTechnique::SetCameraPosition(const Vector3& Pos){
	glUniform3f(m_cameraPosLocation, Pos.x, Pos.y, Pos.z);

}

void BillboardTechnique::SetColorTextureUnit(unsigned int TextureUnit){
	glUniform1i(m_colorMapLocation, TextureUnit);
}