#pragma once
#include "../../nclgl/Mesh.h"
#include "../../nclgl/Vector3.h"
#include "../../nclgl/Vector2.h"
#include <vector>

class Button
{
public:
	Button(Mesh*m);
	~Button(void);
	Mesh* getMesh()		{ return mesh; }
	bool checkAndDraw(bool highlighted);

	void	SetDefaultTexture(GLuint tex)	{ defaultTexture = tex; }
	void	SetHighlightTexture(GLuint tex)	{ highlightTexture = tex; }


protected:
	Matrix4		Transform;
	Vector3		modelScale;
	Mesh*		mesh;

	GLuint		defaultTexture;
	GLuint		highlightTexture;
};

