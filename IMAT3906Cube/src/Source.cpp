#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "glmIncludes.h"

#include "Shader.h"
#include "Camera.h"
#include "renderer.h"
#include "skybox.h"
#include<string>
#include <iostream>
#include <numeric>



// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

//FBO stuff
unsigned int myFBO, myFBODepth;
unsigned int shadowMapFBO;
unsigned int shadowTexture;
unsigned int frameTexture[3];
unsigned int depthAttachment;
unsigned int colorAndDepthBuffer;
unsigned int FBO_blur;
unsigned int blurredTexture;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

void SetUniforms(Shader& shader, Shader& shader2);

void SetFrameBufferObjectColorAndDepthAttachments();
void SetShadowMapDepthFrameBufferObject();
void SetBlurFBOAndBlurTexture();

// camera
Camera camera(glm::vec3(0,0,9));
Skybox skybox;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//arrays
unsigned int floorVBO, cubeVBO, floorEBO, cubeEBO, cubeVAO, floorVAO;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float orthoSize = 10.f;

// something wrong with my 3d math maybe?
glm::vec3 LightDirectionVector = glm::vec3(-0.2f, -1.f, 0.0f);
glm::vec3 lightPositionPoint = LightDirectionVector * glm::vec3(-1.0f);

glm::mat4 lightProjectionOrthographicMatrix = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, -10.f, 20.f);
glm::mat4 lightViewMatrix = glm::lookAt(lightPositionPoint, glm::vec3(0.0f), glm::vec3(0.0, 1.0f, 0.0));
glm::mat4 lightSpaceMatrix = lightProjectionOrthographicMatrix * lightViewMatrix;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "IMAT3907", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	Renderer renderer(SCR_WIDTH, SCR_HEIGHT);

	// simple vertex and fragment shader 
	Shader cubeShader("..\\shaders\\plainVert.vs", "..\\shaders\\plainFrag.fs");
	Shader planeShader("..\\shaders\\plainVert.vs", "..\\shaders\\planeFrag.fs");
	Shader lightsShader("..\\shaders\\lightsShader.vs", "..\\shaders\\lightsShader.fs");
	Shader postProcess("..\\shaders\\postProcess.vs", "..\\shaders\\postProcessFrag.fs");
	Shader depthPostProcess("..\\shaders\\postProcess.vs", "..\\shaders\\DepthPostProcess.fs");
	Shader depthShadows("..\\shaders\\postProcess.vs", "..\\shaders\\DepthPostProcessForOrtho.fs");
	Shader blur("..\\shaders\\postProcess.vs", "..\\shaders\\blur.fs");
	Shader bloom("..\\shaders\\postProcess.vs", "..\\shaders\\bloom.fs");
	Shader shadyShader("..\\shaders\\shadyShader.vs", "..\\shaders\\shadyShader.fs");
	Shader skyboxShader("..\\shaders\\skybox.vs", "..\\shaders\\skybox.fs");

	//this is bad, separate in own class later
	SetFrameBufferObjectColorAndDepthAttachments();
	SetShadowMapDepthFrameBufferObject();
	SetBlurFBOAndBlurTexture();
	SetUniforms(cubeShader, planeShader);
	skybox.CreateSkybox();

	cubeShader.use();
	planeShader.use();
	postProcess.use();
	postProcess.setInt("image", 0);
	depthPostProcess.use();
	depthPostProcess.setInt("depthMap", 0);
	blur.use();
	blur.setInt("image", 0);
	bloom.use();
	bloom.setInt("image", 0);
	bloom.setInt("bloomBlur", 1);
	bloom.setInt("depthMap", 2);
	depthShadows.use();
	depthShadows.setInt("depthMap", 0);
	skyboxShader.use();

	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		// 1 pass for shadow depth
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shadyShader.use();	// leaving this here because i might have day/night cycle
		shadyShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		renderer.RenderPlane(shadyShader, shadowTexture);
		renderer.RenderCube(shadyShader, shadowTexture);

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		// 2 pass for depth
		glBindFramebuffer(GL_FRAMEBUFFER, myFBODepth);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.RenderScene(cubeShader, planeShader, skyboxShader, camera, skybox);

		// 3 pass for color
		glBindFramebuffer(GL_FRAMEBUFFER, colorAndDepthBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
		renderer.RenderScene(cubeShader, planeShader,skyboxShader, camera, skybox);

		// 4 pass for blur
		glBindFramebuffer(GL_FRAMEBUFFER, FBO_blur);
		blur.use();
		blur.setInt("horz", 1);
		renderer.RenderQuad(blur, frameTexture[1]);	// blurs highlights
		blur.setInt("horz", 0);
		renderer.RenderQuad(blur, blurredTexture);	// blurs all

		// 5 pass render to screen
		glDisable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		bloom.use();
		//renderer.RenderQuad(bloom, frameTexture[0]);
		renderer.RenderQuad(bloom, frameTexture[0], blurredTexture, depthAttachment);
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) renderer.RenderQuad(depthShadows, shadowTexture);

		glfwSwapBuffers(window);
		glfwPollEvents();

		cubeShader.setVec3("sLight.position", camera.Position);
		cubeShader.setVec3("sLight.direction", camera.Front);
	}

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void SetUniforms(Shader& shader, Shader& planeShader) {

	//Directional light
	glm::vec3 lightDir = glm::vec3(-0.2, -0.8, 0.0);
	glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);

	//Spot light
	glm::vec3 sLightColor = glm::vec3(1.0, 1.0, 1.0);

	//point Light
	glm::vec3 pLightPos = glm::vec3(0.0, 0.0, -3.0);
	glm::vec3 pLightColor = glm::vec3(1.0, 1.0, 1.0);
	float Kc = 1.0;
	float Kl = 0.1;
	float Ke = 0.1;

	float minBloomBrightness = 0.35f;

	//Cube Shader
	shader.use();
	shader.setVec3("lightCol", lightColor);
	shader.setVec3("lightDirection", LightDirectionVector); // maybe change the vector of this if day/night cycle?
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	shader.setInt("diffuseTexture", 0);
	shader.setInt("normalTexture", 1);
	shader.setInt("specularTexture", 2);
	shader.setInt("shadowMap", 3);	//shadowMap

	shader.setVec3("pLight.position", pLightPos);
	shader.setVec3("pLight.color", pLightColor);

	shader.setFloat("pLight.Kc", Kc);
	shader.setFloat("pLight.Kl", Kl);
	shader.setFloat("pLight.Ke", Ke);

	shader.setVec3("sLight.color", sLightColor);

	shader.setFloat("sLight.inRad", glm::cos(glm::radians(12.5f)));
	shader.setFloat("sLight.outRad", glm::cos(glm::radians(17.5f)));


	//Floor Shader
	planeShader.use();
	planeShader.setVec3("lightCol", lightColor);
	planeShader.setVec3("lightDirection", LightDirectionVector); // maybe change the vector of this if day/night cycle?
	planeShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	planeShader.setInt("diffuseTexture", 0);
	planeShader.setInt("normalTexture", 1);
	planeShader.setInt("specularTexture", 2);
	planeShader.setInt("displacementTexture", 3);	
	planeShader.setInt("shadowMap", 4);	//shadowMap

	planeShader.setVec3("pLight.position", pLightPos);
	planeShader.setVec3("pLight.color", pLightColor);

	planeShader.setFloat("pLight.Kc", Kc);
	planeShader.setFloat("pLight.Kl", Kl);
	planeShader.setFloat("pLight.Ke", Ke);

	planeShader.setVec3("sLight.color", sLightColor);
	planeShader.setFloat("PXscale", 0.0215);
	planeShader.setFloat("bloomBrightness", minBloomBrightness);
	planeShader.setFloat("sLight.inRad", glm::cos(glm::radians(12.5f)));
	planeShader.setFloat("sLight.outRad", glm::cos(glm::radians(17.5f)));
}


void SetShadowMapDepthFrameBufferObject() {
	glGenFramebuffers(1, &shadowMapFBO); //generate frame buffer object
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SetFrameBufferObjectColorAndDepthAttachments() {
	glGenFramebuffers(1, &colorAndDepthBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, colorAndDepthBuffer);

	glGenTextures(2, frameTexture);

	for (int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, frameTexture[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, frameTexture[i], 0);
	}

	glGenTextures(1, &depthAttachment);
	glBindTexture(GL_TEXTURE_2D, depthAttachment);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);

	unsigned int attachment[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachment);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SetBlurFBOAndBlurTexture() {
	glGenFramebuffers(1, &FBO_blur);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_blur);

	glGenTextures(1, &blurredTexture);
	glBindTexture(GL_TEXTURE_2D, blurredTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurredTexture, 0);

}