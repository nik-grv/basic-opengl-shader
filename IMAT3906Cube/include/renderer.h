#pragma once
#include "glmIncludes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include "Shader.h"
#include "Camera.h"
#include "skybox.h"

#include "normalMapper.h"

class Renderer {
public:
	Renderer(unsigned int screenWidth, unsigned int screenHeight);
	void RenderScene(Shader& shader, Shader& planeShader, Shader& skyboxShader, Camera camera, Skybox skybox);
	void RenderQuad(Shader& shader, unsigned int& textureObj);
	void RenderQuad(Shader& shader, unsigned int& textureObj, unsigned int& textureObj2);
	void RenderQuad(Shader& shader, unsigned int& textureObj, unsigned int& textureObj2, unsigned int& textureObj3);

	void RenderCube(Shader& shader);
	void RenderCube(Shader& shader, unsigned int& textureObj);
	void RenderPlane(Shader& shader);
	void RenderPlane(Shader& shader, unsigned int& textureObj);
private:

	void CreateCube();
	void CreatePlane();
	void CreateQuad();
	void ReadTextureFiles();

	unsigned int loadTexture(char const* path);

	unsigned int cubeVAO, planeVAO, cubeVBO, cubeEBO, planeVBO, planeEBO, quadVAO, quadVBO;

	unsigned int cubeDiffuse, cubeSpecular, cubeNormal, planeDiffuse, planeSpecular, planeNormal, planeDisplacement;

	// move to floor class later
	float floorSize = 5.0f;
	float floorLevel = -2.0f;

	//move to class later
	glm::vec3 cubeColor = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 planeColor = glm::vec3(1.0, 1.0, 1.0);

	//move to screen class maybe?
	unsigned int SCR_WIDTH, SCR_HEIGHT;
};