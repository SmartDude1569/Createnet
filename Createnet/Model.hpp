#pragma once

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <iostream>

#include "PhysicalInstance.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

class Model : public PhysicalInstance {
public:
	Model(const char* pathToModel, bool shouldLoadModel = true) {
		this->path = pathToModel;
		if (shouldLoadModel) {
			loadModel(pathToModel);
		}
	}
	void draw(Shader& shader) {
		for (unsigned int i = 0; i < meshes.size(); i++) {
			meshes[i].draw(shader);
		}
	}
	const char* getPath() { return path; }
	void setIsColored(bool isColored) { this->isColored = isColored; }
	bool getIsColored() { return isColored; }
	void setColor(glm::vec3 color) { this->color = color; }
	glm::vec3 getColor() { return color; }
	void setSpecular(glm::vec3 specular) { this->specular = specular; }
	glm::vec3 getSpecular() { return specular; }
private:
	bool isColored;
	glm::vec3 color;
	glm::vec3 specular;
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	const char* path;
	void loadModel(const char* pathToModel) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(pathToModel, aiProcess_Triangulate);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "[ASSIMP] couldn't load model \"" << pathToModel << "\": " << importer.GetErrorString() << std::endl;
			return;
		}
		directory = std::string(path).substr(0, std::string(path).find_last_of('/'));
		processNode(scene->mRootNode, scene);
	}
	void processNode(aiNode* node, const aiScene* scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}
	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			glm::vec3 v;
			v.x = mesh->mVertices[i].x;
			v.y = mesh->mVertices[i].y;
			v.z = mesh->mVertices[i].z;
			vertex.position = v;
			v.x = mesh->mNormals[i].x;
			v.y = mesh->mNormals[i].y;
			v.z = mesh->mNormals[i].z;
			vertex.normal = v;
			if (mesh->mTextureCoords[0]) {
				glm::vec2 vtc;
				vtc.x = mesh->mTextureCoords[0][i].x;
				vtc.y = mesh->mTextureCoords[0][i].y;
				vertex.texCoords = vtc;
			}
			else vertex.texCoords = glm::vec2(0, 0);
			vertices.push_back(vertex);
		}
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		}
		return Mesh(vertices, indices, textures);
	}
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++) {
				if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0) {
					textures.push_back(textures_loaded[j]);
					skip = true;
				}
			}
			if (!skip) {
				Texture texture = loadTexture(str.C_Str(), directory);
				texture.type = typeName;
				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}
		}
		return textures;
	}
};