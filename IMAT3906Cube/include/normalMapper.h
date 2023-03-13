#ifndef NORMALMAPPER_H
#define NORMALMAPPER_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glmincludes.h"
#include <vector>

struct vertex {
	glm::vec3 pos;
	glm::vec3 normals;
	glm::vec2 uv;
	glm::vec3 tan;
	glm::vec3 bitan;
};

class NormalMapper {
	

public:
	NormalMapper();
	void CalculateTanAndBitan(float* vertexData, int lenght, unsigned int* indicesData, int indicesLenght);
	
	std::vector<float> GetUpdatedVertexData();
private:
	std::vector<vertex> vertecies;
	std::vector<float> updatedVertexData;
	void UnpackVertecies();
	void ExtractVertecies(float* vertexData, int lenght);
	void ComputeTanAndBitan(unsigned int* indicesData, int indicesLenght);
};

#endif