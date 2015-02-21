#include "Frustum.h"

void Frustum::FromMatrix(const Matrix4 &mat) 
{


	//Vector3 xaxis = Vector3(mat.values[0], mat.values[4], mat.values[8]);
	//Vector3 yaxis = Vector3(mat.values[1], mat.values[5], mat.values[9]);
	//Vector3 zaxis = Vector3(mat.values[2], mat.values[6], mat.values[10]);
	//Vector3 waxis = Vector3(mat.values[3], mat.values[7], mat.values[11]);
	Vector3 xaxis = mat.getRow(0).getXYZ();
	Vector3 yaxis = mat.getRow(1).getXYZ();
	Vector3 zaxis = mat.getRow(2).getXYZ();
	Vector3 waxis = mat.getRow(3).getXYZ();

	// 0	4	8	12
	// 1	5	9	13
	// 3	6	10	14
	// 4	7	11	15

	//RIGHT
	planes[0] = Plane(waxis - xaxis, (mat.getElem(3,3) - mat.getElem(3,0)), true);

	//LEFT
	planes[1] = Plane(waxis + xaxis, (mat.getElem(3,3) + mat.getElem(3,0)), true);

	//BOTTOM
	planes[2] = Plane(waxis + yaxis, (mat.getElem(3,3) + mat.getElem(3,1)), true);

	//TOP
	planes[3] = Plane(waxis - yaxis, (mat.getElem(3,3) - mat.getElem(3,1)), true);

	//FAR
	planes[4] = Plane(waxis - zaxis, (mat.getElem(3,3) - mat.getElem(3,2)), true);

	//NEAR
	planes[5] = Plane(waxis + zaxis, (mat.getElem(3,3) + mat.getElem(3,2)), true);
}

bool Frustum::InsideFrustum(SceneNode&n)	
{
	for (int p = 0; p < 6; p++)	
	{
		//if(!planes[p].PointInPlane(n.GetWorldTransform().GetPositionVector())) 
		//if(!planes[p].SphereInPlane(n.GetWorldTransform().GetPositionVector(), n.GetBoundingRadius()))
		if (!planes[p].SphereInPlane(n.GetWorldTransform().getTranslation(), n.GetBoundingRadius()))
		{
			return false;
		}
	}
	return true;
}