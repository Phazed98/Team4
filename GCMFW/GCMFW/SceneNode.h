#pragma once

#include <vector>

class Mesh;
#include <vectormath/cpp/vectormath_aos.h>
#include "GCMRenderer.h"
#include "Shader.h"

using namespace Vectormath::Aos;

class SceneNode	{
public:
	SceneNode(Mesh*m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	void			SetTransform(const Matrix4 &matrix) { transform = matrix; }
	const Matrix4&	GetTransform() const				{ return transform; }
	Matrix4			GetWorldTransform() const			{ return worldTransform; }

	virtual void	Update(float msec);
	//virtual void	Draw(VertexShader &vertex, FragmentShader &fragment);

	Vector4			GetColour()		const			{ return colour; }
	void			SetColour(const Vector4 &c)		{ colour = c; }

	Vector3			GetModelScale()		const			{ return modelScale; }
	void			SetModelScale(const Vector3 &s)		{ modelScale = s; }

	void			AddChild(SceneNode* s);
	bool			RemoveChild(SceneNode* s, bool recursive = true);

	float			GetBoundingRadius() const	{ return boundingRadius; }
	void			SetBoundingRadius(float f)	{ boundingRadius = f; }

	float			GetCameraDistance() const	{ return distanceFromCamera; }
	void			SetCameraDistance(float f)	{ distanceFromCamera = f; }

	void			SetMesh(Mesh*m)						{ mesh = m; }
	Mesh*			GetMesh()							{ return mesh; }

	bool	IsAwake()	{ return awake; }
	void	Wake()		{ awake = true; }
	void	Sleep()		{ awake = false; }

	static bool		CompareByCameraDistance(SceneNode*a, SceneNode*b);
	static bool		CompareByZ(SceneNode*a, SceneNode*b);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart()	{ return children.begin(); }
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd()	{ return children.end(); }



protected:
	Mesh*		mesh;
	SceneNode*	parent;
	Matrix4		worldTransform;
	Matrix4		transform;
	Vector3		modelScale;
	Vector4		colour;
	float		distanceFromCamera;
	float		boundingRadius;

	bool		awake;

	std::vector<SceneNode*>		children;
};
