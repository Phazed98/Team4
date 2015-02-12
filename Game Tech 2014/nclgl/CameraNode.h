/******************************************************************************
Class:CameraNode
Implements:SceneNode, Camera
Author:Rich Davison
Description: A SceneNode wrapper for the Camera class. It seemed beneficial to
allow the Renderer and SoundSystem to be able to render from the POV of any
node in the SceneGraph, rather than being limited to a separate Camera object,
so the easiest solution was to smush SceneNode and Camera together. 

Note, the Update function will create incorrect transformation matrices if
the CameraNode is attached to anything other than the root node. This is
an easy fix, and one that would allow your Camera to automatically follow a 
SceneNode if it was attached as a child. Could be useful?

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////


#pragma once
#include "scenenode.h"
#include "Camera.h"
class CameraNode : public SceneNode, public Camera {
public:
	CameraNode(void){};
	~CameraNode(void){};

	virtual void	Update(float msec) {
		UpdateCamera(msec);	//Camera class update

		transform = Matrix4::Translation(position) * Matrix4::Rotation(yaw, Vector3(0,1,0)) * Matrix4::Rotation(pitch, Vector3(1,0,0));
					
		SceneNode::Update(msec);	//SceneNode class update
	}
};