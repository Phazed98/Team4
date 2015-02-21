#pragma once
//#include "common.h"
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include "Mesh.h"
#include "ChildMeshInterface.h"
#include <vectormath/cpp/vectormath_aos.h>

#define OBJOBJECT		"object"	//the current line of the obj file defines the start of a new material
#define OBJMTLLIB		"mtllib"
#define OBJUSEMTL		"usemtl"	//the current line of the obj file defines the start of a new material
#define OBJMESH			"g"			//the current line of the obj file defines the start of a new face
#define OBJCOMMENT		"#"			//The current line of the obj file is a comment
#define OBJVERT			"v"			//the current line of the obj file defines a vertex
#define OBJTEX			"vt"		//the current line of the obj file defines texture coordinates
#define OBJNORM			"vn"		//the current line of the obj file defines a normal
#define OBJFACE			"f"			//the current line of the obj file defines a face

#define MTLNEW			"newmtl"
#define MTLDIFFUSE		"Kd"
#define MTLSPEC			"Ks"
#define MTLSPECWT		"Ns"
#define MTLTRANS		"d"
#define MTLTRANSALT		"Tr"
#define MTLILLUM		"illum"
#define MTLDIFFUSEMAP	"map_Kd"
#define MTLBUMPMAP		"map_bump"
#define MTLBUMPMAPALT	"bump"



/*
OBJSubMesh structs are used to temporarily keep the data loaded
in from the OBJ files, before being parsed into a series of
Meshes
*/
struct OBJSubMesh 
{
	std::vector<int> texIndices;
	std::vector<int> vertIndices;
	std::vector<int> normIndices;

	int indexOffset;
	std::string mtlType;
	std::string mtlSrc;
};

struct MTLInfo 
{
	std::string bump;
	std::string diffuse;

	CellGcmTexture* bumpNum;
	CellGcmTexture* diffuseNum;

	MTLInfo() 
	{
		bumpNum = 0;
		diffuseNum = 0;
	}
	//this is all we care about...
};

class OBJMesh : public Mesh, public ChildMeshInterface	
{
public:
	OBJMesh(void){};
	OBJMesh(std::string filename)
	{ 
		LoadOBJMesh(filename); 
	};

	~OBJMesh(void){};
	bool	LoadOBJMesh(std::string filename);

	virtual void	Draw(VertexShader &vertex, FragmentShader &fragment);

protected:
	void	SetTexturesFromMTL(std::string &mtlFile, std::string &mtlType);

	void	FixTextures(MTLInfo &info);

	std::map <std::string, MTLInfo> materials;
};