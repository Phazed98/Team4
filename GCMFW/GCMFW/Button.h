#pragma once
#include <vector>
#include "Mesh.h"
#include <vectormath/cpp/vectormath_aos.h>
#include "Renderer.h"

using namespace Vectormath::Aos;


class Button : public SceneNode
{

public:
	Button(Mesh*m);
	~Button(void);
	Mesh* getMesh()		{ return mesh; }
	bool checkAndDraw(bool highlighted, VertexShader &vertex, FragmentShader &fragment);

	//void	SetDefaultTexture(GLuint tex)	{ defaultTexture = tex; }
	//void	SetHighlightTexture(GLuint tex)	{ highlightTexture = tex; }

	void	SetDefaultTexture(CellGcmTexture &def)  { defaultTexture = &def; }
	void	SetHighlightTexture(CellGcmTexture &high)  { highlightTexture = &high; }

	const CellGcmTexture*	GetDefaultTexture()						{ return defaultTexture; }
	const CellGcmTexture*	GetHighlightTexture()						{ return highlightTexture; }

protected:
	Matrix4		Transform;
	Vector3		modelScale;
	Mesh*		mesh;

	CellGcmTexture* defaultTexture;	//Default texture for this mesh, if any
	CellGcmTexture* highlightTexture;

	//GLuint		defaultTexture;
	//GLuint		highlightTexture;
};

