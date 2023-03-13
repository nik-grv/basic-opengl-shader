#include "..\include\renderer.h"

Renderer::Renderer(unsigned int screenWidth, unsigned int screenHeight)
{
	SCR_WIDTH = screenWidth;
	SCR_HEIGHT = screenHeight;
	ReadTextureFiles();
	CreateCube();
	CreatePlane();
	CreateQuad();
}

void Renderer::RenderScene(Shader& shader, Shader& planeShader, Shader& skyboxShader, Camera camera, Skybox skybox)
{
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	shader.use();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);
	shader.setVec3("cameraPosition", camera.Position);

	planeShader.use();
	planeShader.setMat4("projection", projection);
	planeShader.setMat4("view", view);
	planeShader.setMat4("model", model);
	planeShader.setVec3("cameraPosition", camera.Position);

	skyboxShader.use();
	skyboxShader.setMat4("projection", projection);
	skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));

	skybox.DrawSkybox(skyboxShader);
	RenderCube(shader);
	RenderPlane(planeShader);
}

void Renderer::RenderCube(Shader& shader)
{
	shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	shader.setVec3("objectCol", cubeColor);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, cubeNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, cubeSpecular);

	glBindVertexArray(cubeVAO);  // bind and draw cube
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.15, 0.0, 3.0)); //change model matrix to do stuff
	shader.setMat4("model", model);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = glm::mat4(1.0f); // reset model matrix before doing other stuff

	model = glm::translate(model, glm::vec3(-1.3, 0.0, -3.0));
	//model = glm::rotate(model, float(glfwGetTime()), glm::vec3(2.0, 0.0, 0.0));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setMat4("model", model);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Renderer::RenderCube(Shader& shader, unsigned int& textureObj)
{
	shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	shader.setVec3("objectCol", cubeColor);
	shader.setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, cubeNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, cubeSpecular);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureObj);

	glBindVertexArray(cubeVAO);  // bind and draw cube
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.15, 0.0, 3.0)); //change model matrix to do stuff
	shader.setMat4("model", model);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	model = glm::mat4(1.0f); // reset model matrix before doing other stuff

	model = glm::translate(model, glm::vec3(-1.3, 0.0, -3.0));
	//model = glm::rotate(model, float(glfwGetTime()), glm::vec3(2.0, 0.0, 0.0));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setMat4("model", model);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Renderer::RenderPlane(Shader& shader)
{
	shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);
	shader.setVec3("objectCol", planeColor);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planeDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, planeNormal); 
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, planeSpecular);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, planeDisplacement);

	glBindVertexArray(planeVAO);  // bind and draw floor
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);   // what happens if we change to GL_LINE?
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}

void Renderer::RenderPlane(Shader& shader, unsigned int& textureObj)
{
	shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);
	shader.setVec3("objectCol", planeColor);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planeDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, planeNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, planeSpecular);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, planeDisplacement);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureObj);

	glBindVertexArray(planeVAO);  // bind and draw floor
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);   // what happens if we change to GL_LINE?
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}

void Renderer::RenderQuad(Shader& shader, unsigned int& textureObj)	//this can go to the frame buffer class
{
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::RenderQuad(Shader& shader, unsigned int& textureObj, unsigned int& textureObj2)	//this can go to the frame buffer class
{
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureObj2);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::RenderQuad(Shader& shader, unsigned int& textureObj, unsigned int& textureObj2, unsigned int& textureObj3)	//this can go to the frame buffer class
{
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureObj2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureObj3);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::CreateCube()
{	
	float cubeVertices[] = {
		//back
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,// 0 
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, //1
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

		//front
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,//4
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,

		//left
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,//8
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		//right
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f,//12
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		//bottom
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,//16
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,

		//top	
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, //20
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
	};

	unsigned int cubeIndices[] = {
		1,2,3,
		1,3,0,

		5,6,7,
		5,7,4,

		11,8,9,
		11,9,10,

		15,12,13,
		15,13,14,

		17, 18, 19,
		16, 17, 19,

		23,22,21,
		23,21,20
	};

	NormalMapper normalMapper;
	normalMapper.CalculateTanAndBitan(cubeVertices, 192, cubeIndices, 36);	//remove hardcoded values; 192 = cube vertecies; 36 = cube indecies;
	std::vector<float> updatedCubeVertecies = normalMapper.GetUpdatedVertexData();
	// Cube
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);
	// fill VBO with vertex data
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, updatedCubeVertecies.size() * sizeof(GLfloat), updatedCubeVertecies.data(), GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	// fill EBO with index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// tangent attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	// bitangent attribute
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);
}

void Renderer::CreatePlane()
{
	float floorVertices[] = {
		-floorSize, floorLevel,  -floorSize, 0.0, 1.0, 0.0, 0.0f, 0.0f,
		 floorSize, floorLevel,   -floorSize, 0.0, 1.0, 0.0, 1.0f, 0.0f,
		 floorSize, floorLevel,  floorSize, 0.0, 1.0, 0.0, 1.0f,1.0f,
		-floorSize, floorLevel,  floorSize, 0.0, 1.0, 0.0, 0.0f,1.0f

	};

	unsigned int floorIndices[] = {
		3,2,1,
		3,1,0
	};

	NormalMapper normalMapper;
	normalMapper.CalculateTanAndBitan(floorVertices, 32, floorIndices,	6);	//remove hardcoded values; 192 = cube vertecies; 36 = cube indecies;
	std::vector<float> updatedFloorVertecies = normalMapper.GetUpdatedVertexData();

	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glGenBuffers(1, &planeEBO);

	glBindVertexArray(planeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, updatedFloorVertecies.size() * sizeof(GLfloat), updatedFloorVertecies.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// tangent attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	// bitangent attribute
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);
}

void Renderer::CreateQuad()
{
	float quadVerts[] = {
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), &quadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));

}

unsigned int Renderer::loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1) format = GL_RED;
		if (nrComponents == 3) format = GL_RGB;
		if (nrComponents == 4) format = GL_RGBA;


		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
		std::cout << " Loaded texture at path" << path << " width " << width << " id " << textureID << std::endl;
	}

	else
	{
		std::cout << " Texture failed to load at path " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}



void Renderer::ReadTextureFiles() {

	cubeDiffuse = loadTexture("..\\Resources\\quartz\\quartzDiffuse.jpg");
	cubeNormal = loadTexture("..\\Resources\\quartz\\quartzNormal.jpg");
	cubeSpecular = loadTexture("..\\Resources\\quartz\\quartzSpecular.jpg");

	planeDiffuse = loadTexture("..\\Resources\\stoneTiles\\stoneDiffuse.jpg");
	planeNormal = loadTexture("..\\Resources\\stoneTiles\\stoneNormal.jpg");
	planeSpecular = loadTexture("..\\Resources\\stoneTiles\\stoneSpecular.jpg");
	planeDisplacement = loadTexture("..\\Resources\\stoneTiles\\stoneDisplacement3.jpg");

}

