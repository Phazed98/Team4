#pragma once
#include "../../nclgl/Mesh.h"
#include "../../nclgl/Vector3.h"
#include "../../nclgl/Vector2.h"
#include <vector>

class Button	
{
public:
	Button(Mesh*m, Vector2 minPos, Vector2 maxPos);
	~Button(void);

	Vector2 getMinPos() { return minPos; }
	Vector2 getMaxPos()	{ return maxPos; }
	Mesh* getMesh()		{ return mesh; }
	bool checkAndDraw(Vector2 mosVec, bool mouseClicked);

	void	SetDefaultTexture(GLuint tex)	{ defaultTexture = tex; }
	void	SetHighlightTexture(GLuint tex)	{ highlightTexture = tex; }
	void	SetPressedTexture(GLuint tex)	{ pressedTexture = tex; }


protected:
	Matrix4		Transform;
	Vector3		modelScale;
	Vector2		minPos;
	Vector2		maxPos;
	Mesh*		mesh;

	GLuint		defaultTexture;
	GLuint		highlightTexture;
	GLuint		pressedTexture;
};

