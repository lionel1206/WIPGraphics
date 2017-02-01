#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "assimp\cimport.h"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

#include "rapidjson\prettywriter.h"
#include "rapidjson\stringbuffer.h"


struct vector2D
{
	float x;
	float y;
	vector2D(float _x, float _y) : x(_x), y(_y) {}
};

struct vector3D
{
	float x;
	float y;
	float z;
	vector3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct meshData
{
	std::string meshName;
	std::vector<vector3D> uvs;
	std::vector<vector3D> verts;
	std::vector<int> faces;
	std::vector<vector3D> normals;
	std::vector<vector3D> tans;
	std::vector<vector3D> biTans;
};

int main(char argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Input count " << argc << std::endl;
		std::cout << "Usage: exe <input model file> <output model.json>" << std::endl;
		return 0;
	}

	std::cout << "Reading from " << argv[1] << std::endl;
	std::cout << "writing to " << argv[2] << std::endl;

	// open the model file
	const aiScene* scene = aiImportFile(argv[1], aiProcessPreset_TargetRealtime_Quality);
	if (scene == nullptr)
	{
		std::cout << "Cannot read " << argv[1] << std::endl;
		std::cout << aiGetErrorString() << std::endl;
		std::cout << "Exiting..." << std::endl;
		return 0;
	}

	std::vector<meshData> modelData;
	// read the meshes in the model
	for (size_t index = 0; index < scene->mNumMeshes; ++index)
	{
		aiMesh* currentMesh = scene->mMeshes[index];
		meshData meshData;
		std::string meshName(currentMesh->mName.C_Str());

		if (currentMesh->HasPositions())
		{
			// get all the vertices if any
			for (size_t i = 0; i < currentMesh->mNumVertices; ++i)
			{
				aiVector3D vertice = currentMesh->mVertices[i];
				meshData.verts.push_back(vector3D(vertice.x,
					vertice.y,
					vertice.z));

				if (currentMesh->HasTextureCoords(0))
				{
					aiVector3D texCoord = currentMesh->mTextureCoords[0][i];
					meshData.uvs.push_back(vector3D(texCoord.x,
						texCoord.y,
						texCoord.z));
				}

				if (currentMesh->HasTangentsAndBitangents())
				{
					aiVector3D tangent = currentMesh->mTangents[i];
					aiVector3D biTangent = currentMesh->mBitangents[i];
					meshData.tans.push_back(vector3D(tangent.x,
						                             tangent.y,
						                             tangent.z));
					meshData.biTans.push_back(vector3D(biTangent.x,
												       biTangent.y,
												       biTangent.z));
				}
			}
		}

		if (currentMesh->HasFaces())
		{
			for (size_t i = 0; i < currentMesh->mNumFaces; ++i)
			{
				aiFace currentFace = currentMesh->mFaces[i];
				// as aiProcessPreset_TargetRealtime_Quality has the aiProcess_Triangulate flag set,
				// there are only 3 indices per face
				for (int j = 0; j < currentFace.mNumIndices; ++j)
				{
					meshData.faces.push_back(currentFace.mIndices[j]);
				}	
			}
		}

		if (currentMesh->HasNormals())
		{
			for (size_t i = 0; i < currentMesh->mNumVertices; ++i)
			{
				aiVector3D normal = currentMesh->mNormals[i];
				meshData.normals.push_back(vector3D(normal.x,
					normal.y,
					normal.z));
			}
		}

		modelData.push_back(meshData);
	}

	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> w(s);
	w.StartObject();
	w.Key("ModelFile");
	w.String(argv[1]);
	//w.Key("MeshData");
	//w.StartArray();
	for (size_t i = 0; i < modelData.size(); ++i)
	{
		meshData mesh = modelData[i];
		/*std::stringstream ss;
		ss << "Mesh" << i;
		w.Key(ss.str().c_str());
		w.StartArray();*/
			w.Key("MeshName");
			w.String(mesh.meshName.c_str());
			w.Key("Vertices");
			w.StartArray();
				for (size_t j = 0; j < mesh.verts.size(); ++j)
				{
					w.StartArray();
					w.Double(mesh.verts[j].x);
					w.Double(mesh.verts[j].y);
					w.Double(mesh.verts[j].z);
					w.EndArray();
				}
			w.EndArray();

			w.Key("Indices");
			w.StartArray();
				for (size_t j = 0; j < mesh.faces.size(); ++j)
				{
					w.Int(mesh.faces[j]);
				}
			w.EndArray();

			w.Key("Normals");
			w.StartArray();
				for (size_t j = 0; j < mesh.normals.size(); ++j)
				{
					w.StartArray();
					w.Double(mesh.normals[j].x);
					w.Double(mesh.normals[j].y);
					w.Double(mesh.normals[j].z);
					w.EndArray();
				}
			w.EndArray();

			w.Key("TexCoords");
			w.StartArray();
				for (size_t j = 0; j < mesh.uvs.size(); ++j)
				{
					w.StartArray();
					w.Double(mesh.uvs[j].x);
					w.Double(mesh.uvs[j].y);
					w.Double(mesh.uvs[j].z);
					w.EndArray();
				}
			w.EndArray();

			w.Key("Tangents");
			w.StartArray();
				for (size_t j = 0; j < mesh.tans.size(); ++j)
				{
					w.StartArray();
					w.Double(mesh.tans[j].x);
					w.Double(mesh.tans[j].y);
					w.Double(mesh.tans[j].z);
					w.EndArray();
				}
			w.EndArray();

			w.Key("BiTangents");
			w.StartArray();
				for (size_t j = 0; j < mesh.biTans.size(); ++j)
				{
					w.StartArray();
					w.Double(mesh.biTans[j].x);
					w.Double(mesh.biTans[j].y);
					w.Double(mesh.biTans[j].z);
					w.EndArray();
				}
			w.EndArray();
		//w.EndArray();
	}
	//w.EndArray();
	w.EndObject();

	std::ofstream outFile(argv[2], std::ofstream::out);
	outFile << s.GetString() << std::endl;
	outFile.close();

	
}