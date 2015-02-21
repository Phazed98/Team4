#include "OBJMesh.h"

bool	OBJMesh::LoadOBJMesh(std::string filename)	
{
	std::string folder = SYS_APP_HOME + filename;

	std::cout << "Loading Mesh: " << folder << std::endl;
	std::ifstream f(folder.c_str(), std::ios::in);

	if (!f) //Oh dear, it can't find the file :(
	{
		std::cout << "Failed to Load file" << std::endl;
		return false;
	}

	/*
	Stores the loaded in vertex attributes
	*/
	std::vector<Vector3>inputTexCoords;
	std::vector<Vector3>inputVertices;
	std::vector<Vector3>inputNormals;

	/*
	SubMeshes temporarily get kept in here
	*/
	std::vector<OBJSubMesh*> inputSubMeshes;

	OBJSubMesh* currentMesh = new OBJSubMesh();
	inputSubMeshes.push_back(currentMesh);	//It's safe to assume our OBJ will have a mesh in it ;)

	std::string currentMtlLib;
	std::string currentMtlType;

	while (!f.eof()) 
	{
		std::string currentLine;
		f >> currentLine;

		if (currentLine == OBJCOMMENT) //This line is a comment, ignore it
		{		
			continue;
		}
		else if (currentLine == OBJMTLLIB)
		{
			f >> currentMtlLib;
		}
		else if (currentLine == OBJUSEMTL) 
		{
			currentMesh = new OBJSubMesh();
			inputSubMeshes.push_back(currentMesh);

			currentMesh->mtlSrc = currentMtlLib;

			f >> currentMtlType;

			currentMesh->mtlType = currentMtlType;
		}
		else if (currentLine == OBJMESH || currentLine == OBJOBJECT) //This line is a submesh!
		{	
			currentMesh = new OBJSubMesh();
			inputSubMeshes.push_back(currentMesh);

			currentMesh->mtlSrc = currentMtlLib;
			currentMesh->mtlType = currentMtlType;
		}
		else if (currentLine == OBJVERT) //This line is a vertex
		{	
			Vector3 vertex;
			float xCoord, yCoord, zCoord;
			f >> xCoord; f >> yCoord; f >> zCoord;
			vertex.setX(xCoord);
			vertex.setY(yCoord);
			vertex.setZ(zCoord);
			inputVertices.push_back(vertex);
		}
		else if (currentLine == OBJNORM) //This line is a Normal!
		{	
			Vector3 normal;
			float xNorm, yNorm, zNorm;
			f >> xNorm; f >> yNorm; f >> zNorm;
			normal.setX(xNorm);
			normal.setY(yNorm);
			normal.setZ(zNorm);
			inputNormals.push_back(normal);
		}
		else if (currentLine == OBJTEX) //This line is a texture coordinate!
		{	
			Vector3 texCoord;
			float xTex, yTex;
			f >> xTex; f >> yTex;
			
			texCoord.setX(xTex);
			texCoord.setY(yTex);
			inputTexCoords.push_back(texCoord);
		}
		else if (currentLine == OBJFACE) //This is an object face!
		{	
			if (!currentMesh) 
			{
				inputSubMeshes.push_back(new OBJSubMesh());
				currentMesh = inputSubMeshes[inputSubMeshes.size() - 1];
			}

			std::string			faceData;		//Keep the entire line in this!
			std::vector<int>	faceIndices;	//Keep the extracted indices in here!

			getline(f, faceData);		//Use a std::string helper function to read in the entire face line

			/*
			It's possible an OBJ might have normals defined, but not tex coords!
			Such files should then have a face which looks like this:

			f <vertex index>//<normal index>
			instead of <vertex index>/<tex coord>/<normal index>

			you can be some OBJ files will have "/ /" instead of "//" though... :(
			*/
			bool	skipTex = false;
			if (faceData.find("//") != std::string::npos) 
			{
				skipTex = true;
			}

			/*
			Count the number of slashes, but also convert the slashes to spaces
			so that std::string streaming of ints doesn't fail on the slash

			"f  0/0/0" becomes "f 0 0 0" etc
			*/
			for (size_t i = 0; i < faceData.length(); ++i) 
			{
				if (faceData[i] == '/') 
				{
					faceData[i] = ' ';
				}
			}

			/*
			Now std::string stream the indices from the std::string into a temporary
			vector.
			*/
			int tempIndex;
			std::stringstream	ss(faceData);
			while (ss >> tempIndex) 
			{
				faceIndices.push_back(tempIndex);
			}

			//If the face indices vector is a multiple of 3, we're looking at triangles
			//with some combination of vertices, normals, texCoords
			if (faceIndices.size() % 3 == 0) //This face is a triangle (probably)!
			{		
				if (faceIndices.size() == 3) //This face has only vertex information;
				{	
					currentMesh->vertIndices.push_back(faceIndices.at(0));
					currentMesh->vertIndices.push_back(faceIndices.at(1));
					currentMesh->vertIndices.push_back(faceIndices.at(2));
				}
				else if (faceIndices.size() == 9) //This face has vertex, normal and tex information!
				{	
					for (int i = 0; i < 9; i += 3) 
					{
						currentMesh->vertIndices.push_back(faceIndices.at(i));
						currentMesh->texIndices.push_back(faceIndices.at(i + 1));
						currentMesh->normIndices.push_back(faceIndices.at(i + 2));
					}
				}
				else if (faceIndices.size() == 6) //This face has vertex, and one other index...
				{	
					for (int i = 0; i < 6; i += 2) 
					{
						currentMesh->vertIndices.push_back(faceIndices.at(i));
						if (!skipTex) // a double slash means it's skipping tex info...
						{		
							currentMesh->texIndices.push_back(faceIndices.at(i + 1));
						}
						else
						{
							//currentMesh->texIndices.push_back(faceIndices.at(i + 1));
							currentMesh->normIndices.push_back(faceIndices.at(i + 1));
						}
					}
				}
			}
			else //Uh oh! Face isn't a triangle. Have fun adding stuff to this ;)
			{
				bool a = true;
			}
		}
		else
		{
			std::cout << "OBJMesh::LoadOBJMesh Unknown file data:" << currentLine << std::endl;
		}
	}

	f.close();
	//We now have all our mesh data loaded in...Now to convert it into OpenGL vertex buffers!
	//Screw That ... we Need PS3 mesh information


	/*
	IMPORTANT THIS SHOULD ALL BE EXACTLY THE SAME!!!!!!!!!
	Its just pulling the data out of the OBJ file and putting it in vectors, easy stuff,
	the next part will be where the changes are required as the mesh structure is different
	on the PS3 unfortunatly
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	*/
	//std::cout << "About to Submesh!!!" << std::endl;
	for (unsigned int i = 0; i < inputSubMeshes.size();)
	{
		//std::cout << "Submeshing: " << i << std::endl;
		OBJSubMesh*sm = inputSubMeshes[i];

		/*
		We're going to be lazy and turn the indices into an absolute list
		of vertex attributes (it makes handling the submesh list easier)
		*/
		if (sm->vertIndices.empty())
		{
			delete sm;
			inputSubMeshes.erase(inputSubMeshes.begin() + i);
			continue;
		}
		else
		{
			OBJMesh*m;

			if (i == 0)
			{
				m = this;
			}
			else
			{
				m = new OBJMesh();
			}

			//m->SetTexturesFromMTL(sm->mtlSrc, sm->mtlType);

			m->numVertices = sm->vertIndices.size();
			m->vertexData = (Vertex*)GCMRenderer::localMemoryAlign(128, sizeof(Vertex) * m->numVertices); //allocate some memory

			for (int x = 0; x < sm->vertIndices.size(); ++x)
			{
				m->vertexData[x].x = inputVertices[sm->vertIndices[x] - 1].getX();	//Top of our triangle
				m->vertexData[x].y = inputVertices[sm->vertIndices[x] - 1].getY();
				m->vertexData[x].z = inputVertices[sm->vertIndices[x] - 1].getZ();
				m->vertexData[x].u = inputTexCoords[sm->texIndices[x] - 1].getY();
				m->vertexData[x].v = inputTexCoords[sm->texIndices[x] - 1].getX();
				m->vertexData[x].rgba = 0x00ff00ff;	//Green
				//std::cout << "Loaded Vertex Data: " << x << " of " << sm->vertIndices.size() << std::endl;
			}

			if (i != 0) 
			{
				AddChild(m);
			}
		
			delete inputSubMeshes[i];
			++i;

			cellGcmAddressToOffset(&m->vertexData->x, &m->vertexOffsets[VERTEX_POSITION]);
			cellGcmAddressToOffset(&m->vertexData->rgba, &m->vertexOffsets[VERTEX_COLOUR]);
			cellGcmAddressToOffset(&m->vertexData->u, &m->vertexOffsets[VERTEX_TEXCOORD]);			

			//m->vertices = new Vector3[m->numVertices];

			//for (unsigned int j = 0; j < sm->vertIndices.size(); ++j) 
			//{
			//	m->vertices[j] = inputVertices[sm->vertIndices[j] - 1];
			//}

			//if (!sm->texIndices.empty())	
			//{
			//	m->textureCoords = new Vector2[m->numVertices];
			//	for (unsigned int j = 0; j < sm->texIndices.size(); ++j) 
			//	{
			//		m->textureCoords[j] = inputTexCoords[sm->texIndices[j] - 1];
			//	}
			//}
		}

		std::cout << "LoadOBJ success!" << std::endl;
	}
	return true;
}

void OBJMesh::Draw(VertexShader &vertex, FragmentShader &fragment)
{
	Mesh::Draw(vertex, fragment);
	for (unsigned int i = 0; i < children.size(); ++i) 
	{
		children.at(i)->Draw(vertex, fragment);
	}
};


void	OBJMesh::SetTexturesFromMTL(std::string &mtlFile, std::string &mtlType)
{
	if (mtlType.empty() || mtlFile.empty()) 
	{
		return;
	}

	std::map <std::string, MTLInfo>::iterator i = materials.find(mtlType);

	if (i != materials.end()) 
	{
		if (!i->second.diffuse.empty())	
		{
			defaultTexture = i->second.diffuseNum;
		}

		if (!i->second.bump.empty())	
		{
			//bumpTexture = i->second.bumpNum;
		}

		return;
	}

	std::ifstream f(std::string(MESHDIR + mtlFile).c_str(), std::ios::in);

	if (!f) //Oh dear, it can't find the file :(
	{
		return;
	}

	MTLInfo currentMTL;
	std::string  currentMTLName;

	int mtlCount = 0;

	while (!f.eof()) 
	{
		std::string currentLine;
		f >> currentLine;

		if (currentLine == MTLNEW) 
		{
			if (mtlCount > 0) 
			{

#ifdef OBJ_FIX_TEXTURES
				FixTextures(currentMTL);
#endif

				materials.insert(std::make_pair(currentMTLName, currentMTL));
			}
			currentMTL.diffuse = "";
			currentMTL.bump = "";

			f >> currentMTLName;

			mtlCount++;
		}
		else if (currentLine == MTLDIFFUSEMAP) 
		{
			f >> currentMTL.diffuse;

			if (currentMTL.diffuse.find_last_of('/') != std::string::npos) 
			{
				int at = currentMTL.diffuse.find_last_of('/');
				currentMTL.diffuse = currentMTL.diffuse.substr(at + 1);
			}
			else if (currentMTL.diffuse.find_last_of('\\') != std::string::npos) 
			{
				int at = currentMTL.diffuse.find_last_of('\\');
				currentMTL.diffuse = currentMTL.diffuse.substr(at + 1);
			}

			if (!currentMTL.diffuse.empty()) 
			{
				/*
				TODO FIX !!!!!!!!!!!!!!!!! Need to load textures from an MTL not using soil as PS3 only really supports its on GTF file formatting
				*/
				//currentMTL.diffuseNum = SOIL_load_OGL_texture(string(TEXTUREDIR + currentMTL.diffuse).c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
			}
		}
		else if (currentLine == MTLBUMPMAP || currentLine == MTLBUMPMAPALT)
		{
			f >> currentMTL.bump;

			if (currentMTL.bump.find_last_of('/') != std::string::npos) 
			{
				int at = currentMTL.bump.find_last_of('/');
				currentMTL.bump = currentMTL.bump.substr(at + 1);
			}
			else if (currentMTL.bump.find_last_of('\\') != std::string::npos) 
			{
				int at = currentMTL.bump.find_last_of('\\');
				currentMTL.bump = currentMTL.bump.substr(at + 1);
			}

			if (!currentMTL.bump.empty()) 
			{
				/*
				TODO FIX !!!!!!!!!!!!!!!!! Need to load textures from an MTL not using soil as PS3 only really supports its on GTF file formatting
				*/
				//currentMTL.bumpNum = SOIL_load_OGL_texture(string(TEXTUREDIR + currentMTL.bump).c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
			}
		}
	}

#ifdef OBJ_FIX_TEXTURES
	FixTextures(currentMTL);
#endif

	materials.insert(std::make_pair(currentMTLName, currentMTL));
	SetTexturesFromMTL(mtlFile, mtlType);
}

/*
The mtl files in that big pack of city buildings haven't been exported correctly...
*/
void	OBJMesh::FixTextures(MTLInfo &info) 
{
	if (!info.bumpNum) {

		std::string temp = info.diffuse;

		if (temp[temp.size() - 5] == 'd') {
			temp[temp.size() - 5] = 'n';
		}
		else {
			temp.insert(temp.size() - 4, "_n");
		}

		info.bump = temp;


		/*
		TODO FIX !!!!!!!!!!!!!!!!! Need to load textures from an MTL not using soil as PS3 only really supports its on GTF file formatting
		*/
		//info.bumpNum = SOIL_load_OGL_texture(string(TEXTUREDIR + info.bump).c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
	}
}
