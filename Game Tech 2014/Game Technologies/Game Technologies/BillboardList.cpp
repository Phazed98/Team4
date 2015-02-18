

#include "BillboardList.h"


#define NUM_ROWS 10
#define NUM_COLUMNS 10

BillboardList::BillboardList(){
	
	m_VB = GL_INVALID_VALUE ;
	BiillboardTexture = SOIL_load_OGL_texture("../../Textures/billboardgrass0001.png",
	 SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);

}

BillboardList::~BillboardList(){
	if (m_VB != GL_INVALID_VALUE)
	{
		glDeleteBuffers(1, &m_VB);
	}
}

void BillboardList::CreatePositionBuffer()
{
	Vector3 Positions[NUM_ROWS * NUM_COLUMNS];

	for (unsigned int j = 0; j < NUM_ROWS; j++) {
		for (unsigned int i = 0; i < NUM_COLUMNS; i++) {
			Vector3 Pos((float)i, 0.0f, (float)j);
			Positions[j * NUM_COLUMNS + i] = Pos;
		}
	}

	glGenBuffers(1, &m_VB);
	glBindBuffer(GL_ARRAY_BUFFER, m_VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions), &Positions[0], GL_STATIC_DRAW);
}


void BillboardList::Render(const Matrix4& viewMatrix, const Vector3& CameraPos){
	m_technique.Enable();
	m_technique.SetVP(viewMatrix);
	m_technique.SetCameraPosition(CameraPos);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, BiillboardTexture );

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_VB);
	glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_POINTS, 0, NUM_ROWS*NUM_COLUMNS);

	glDisableVertexAttribArray(0);

	
}