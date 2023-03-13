#include "..\include\entity.h"
#include <iostream>
// refactor class its a mess now

Entity::Entity()
{
}

void Entity::CalculateTanAndBitan(float* vertexData, int lenght, unsigned int* indeciesData, unsigned int indeciesLenght)
{
	ExtractVertecies(vertexData, lenght);
	ComputeTanAndBitan(indeciesData, indeciesLenght);
	UnpackVertecies();
}


std::vector<float> Entity::GetUpdatedVertexData()
{
	return updatedVertex;
}

void Entity::UnpackVertecies()
{
	for (vertex v : vertecies) {
		int stride = 14;
		float vertexData[] = {
			v.pos.x, v.pos.y, v.pos.z,
			v.normal.x, v.normal.y, v.normal.z,
			v.uv.x, v.uv.y,
			v.tan.x, v.tan.y, v.tan.z,
			v.bitan.x, v.bitan.y, v.bitan.z
		};

		for (int i = 0; i < stride; i++) {
			updatedVertex.push_back(vertexData[i]);
		}
	}
}

void Entity::ExtractVertecies(float* vertexData, int lenght)
{
	vertex V;
	for (int i = 0; i < lenght; i += 8)
	{
		V.pos = glm::vec3(vertexData[i], vertexData[i + 1], vertexData[i + 2]);
		V.normal = glm::vec3(vertexData[i + 3], vertexData[i + 4], vertexData[i + 5]);
		V.uv = glm::vec2(vertexData[i + 6], vertexData[i + 7]);
		V.tan = glm::vec3(0.0, 0.0, 0.0);
		V.bitan = glm::vec3(0.0, 0.0, 0.0);

		vertecies.push_back(V);
	}
}

void Entity::ComputeTanAndBitan(unsigned int* indeciesData, unsigned int indeciesLenght)
{
	for (int i = 0; i < indeciesLenght; i += 3)
	{
		vertex& a = vertecies.at(indeciesData[i]);
		std::cout<< "a" << indeciesData[i] << std::endl;
		vertex& b = vertecies.at(indeciesData[i + 1]);
		std::cout << "b" << indeciesData[i+1] << std::endl;
		vertex& c = vertecies.at(indeciesData[i + 2]);
		std::cout << "c" << indeciesData[i+2] << std::endl;

		//calc edges of triangle
		glm::vec3 deltaPos1 = b.pos - a.pos;
		glm::vec3 deltaPos2 = c.pos - a.pos;

		// get differences in uv
		glm::vec2 deltaUV1 = b.uv - a.uv;
		glm::vec2 deltaUV2 = c.uv = a.uv;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x); //cross product

		glm::vec3 Tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 Bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		a.tan = a.tan + Tangent;
		a.bitan = a.bitan + Bitangent;

		b.tan = b.tan + Tangent;
		b.bitan = b.bitan + Bitangent;

		c.tan = c.tan + Tangent;
		c.bitan = c.bitan + Bitangent;
	}

	//NormalizeTanAndBitan();

}

void Entity::NormalizeTanAndBitan() {
	for (vertex& v : vertecies) {
		v.tan = glm::normalize(v.tan);
		v.bitan = glm::normalize(v.bitan);
	}
}
