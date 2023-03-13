#pragma once

#ifndef ENTITY_H
#define ENTITY_H

#include "glmIncludes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

struct vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tan;
	glm::vec3 bitan;
};

class Entity {
public:
	Entity();
	void CalculateTanAndBitan(float* vertexData, int lenght, unsigned int* indeciesData, unsigned int indeciesLenght);
	std::vector<float> GetUpdatedVertexData();
	void UnpackVertecies();
	void ExtractVertecies(float* vertexData, int lenght);
	void ComputeTanAndBitan(unsigned int* indeciesData, unsigned int indeciesLenght);
	void NormalizeTanAndBitan();
	
private:
	std::vector<vertex> vertecies;
	std::vector<float> updatedVertex;

};
#endif