#pragma once

#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include "glmIncludes.h"

class Skybox {
public:
	Skybox();
	void CreateSkybox();
	void DrawSkybox(Shader& shader);

private:
	void LoadCubeMapTextureFiles();
	unsigned int loadSkyboxTextures(std::vector<std::string> faces);
	void CreateVertexArrayObject();

	std::vector<std::string> faces;
	unsigned int skyboxTextureObject;
	unsigned int skyboxVertexArrayObject;

};