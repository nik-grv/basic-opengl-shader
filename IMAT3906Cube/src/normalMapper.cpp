#include "normalMapper.h"

NormalMapper::NormalMapper()
{

}

void NormalMapper::CalculateTanAndBitan(float* vertexData, int lenght, unsigned int* indicesData, int indicesLenght)
{	
	ExtractVertecies(vertexData, lenght);
	ComputeTanAndBitan(indicesData, indicesLenght);
	UnpackVertecies();
}

void NormalMapper::UnpackVertecies()
{
	for (vertex v : vertecies) {
		int stride = 14;
		float vertexData[] = {
			v.pos.x, v.pos.y, v.pos.z,
			v.normals.x, v.normals.y, v.normals.z,
			v.uv.x, v.uv.y,
			v.tan.x, v.tan.y, v.tan.z,
			v.bitan.x, v.bitan.y, v.bitan.z
		};

		for (int i = 0; i < stride; i++)
		{
			updatedVertexData.push_back(vertexData[i]);
		}
	}
}

void NormalMapper::ExtractVertecies(float* vertexData, int lenght)
{
	vertex V;
	for (int i = 0; i < lenght; i += 8)
	{
		V.pos = glm::vec3(vertexData[i], vertexData[i + 1], vertexData[i + 2]);
		V.normals = glm::vec3(vertexData[i + 3], vertexData[i + 4], vertexData[i + 5]);
		V.uv = glm::vec2(vertexData[i + 6], vertexData[i + 7]);
		V.tan = glm::vec3(0.f, 0.f, 0.f);
		V.bitan = glm::vec3(0.f, 0.f, 0.f);
		vertecies.push_back(V);
	}
}

void NormalMapper::ComputeTanAndBitan(unsigned int* indicesData, int indicesLenght)
{
	for (int i = 0; i < indicesLenght; i = i + 3)
	{
		vertex& a = vertecies.at(indicesData[i]);
		vertex& b = vertecies.at(indicesData[i+1]);
		vertex& c = vertecies.at(indicesData[i+2]);

		glm::vec3 deltaPos1 = b.pos - a.pos;
		glm::vec3 deltaPos2 = c.pos - a.pos;

		glm::vec2 deltaUV1 = b.uv - a.uv;
		glm::vec2 deltaUV2 = c.uv - a.uv;

		float r = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));

		glm::vec3 Tangent = ((deltaPos1 * deltaUV2.y) - (deltaPos2 * deltaUV1.y)) * r;
		glm::vec3 Bitangent = ((deltaPos2 * deltaUV1.x) - (deltaPos1 * deltaUV2.x)) * r;

		a.tan = a.tan + Tangent;
		a.bitan = a.bitan + Bitangent;

		b.tan = b.tan + Tangent;
		b.bitan = b.bitan + Bitangent;

		c.tan = c.tan + Tangent;
		c.bitan = c.bitan + Bitangent;
	}

	for (vertex& v : vertecies) {
		v.tan = glm::normalize(v.tan);
		v.bitan = glm::normalize(v.bitan);
	}
}

std::vector<float> NormalMapper::GetUpdatedVertexData()
{
	return updatedVertexData;;
}
