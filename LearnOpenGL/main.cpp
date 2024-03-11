#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/Base64.hpp>

#include "data.h"
#include "shader.h"
#include "camera.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float lastX = 810.0f;
float lastY = 540.0f;
bool firstMouse = true;
Camera cam = Camera(glm::vec3(0.0f, 0.0f, -3.0f));

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouseCallBack(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float offsetX = xpos - lastX;
	float offsetY = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	cam.rotate(offsetX, offsetY);
}

void scrollCallBack(GLFWwindow* window, double xoffset, double yoffset) {
	cam.zoom(yoffset);
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwTerminate();
	float camSpeed = 2.5f * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.translate(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.translate(BACKWORD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.translate(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.translate(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cam.translate(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cam.translate(DOWN, deltaTime);
}

static GLFWwindow* init() {
	// initailize glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "LearnOpenGL", NULL, NULL);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallBack);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetScrollCallback(window, scrollCallBack);
	glEnable(GL_DEPTH_TEST);
	return window;
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureId;
	glGenTextures(1, &textureId);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureId;
}

int main()
{
	GLFWwindow* window = init();

#pragma region Compile Shaders
	Shader shaderBox = Shader("shaders/shader_box.vs", "shaders/shader_box.fs");
	Shader shaderLight = Shader("shaders/shader_light.vs", "shaders/shader_light.fs");
#pragma endregion

#pragma region BuffersSetting
	unsigned int vbo, cubeVao;
	glGenVertexArrays(1, &cubeVao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	
	glBindVertexArray(cubeVao);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int lightVao;
	glGenVertexArrays(1, &lightVao);
	glBindVertexArray(lightVao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
#pragma endregion

#pragma region TextureLoad
	unsigned int diffuseMap = loadTexture("textures/container_diffuse.png");
	unsigned int specularMap = loadTexture("textures/container_specular.png");
	/*unsigned int emissionMap = loadTexture("textures/container_emission.jpg");*/
#pragma endregion
	
	shaderBox.use();
	shaderBox.setInt("material.diffuse", 0);
	shaderBox.setInt("material.specular", 1);
	/*shaderBox.setInt("material.emission", 2);*/
#pragma region RenderLoop
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// 光源
		shaderLight.use();
		glm::vec3 lightColor = glm::vec3(1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // 降低影响
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // 很低的影响
		shaderLight.setVec3("lightColor", lightColor);
		glm::mat4 view = cam.getViewMat();
		glm::mat4 project = glm::perspective(glm::radians(cam.getFovY()),
			(float)1920 / (float)1080, 0.1f, 100.0f);
		shaderLight.setMat4("view", view);
		shaderLight.setMat4("project", project);
		glBindVertexArray(lightVao);
		for (int i = 0; i < std::size(lightPositions); i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, lightPositions[i]);
			model = glm::scale(model, glm::vec3(0.1f));
			shaderLight.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		// 被摄物体
		shaderBox.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, emissionMap);

		shaderBox.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		shaderBox.setFloat("material.shininess", 32.0f);

		shaderBox.setVec3("dLight.ambient", 0.01f, 0.01f, 0.01f);
		shaderBox.setVec3("dLight.diffuse", 0.4f, 0.4f, 0.4f);
		shaderBox.setVec3("dLight.specular", 0.5f, 0.5f, 0.5f);
		shaderBox.setVec3("dLight.direction", -0.2f, -1.0f, -0.3f);

		for (int i = 0; i < std::size(lightPositions); i++) {
			std::string tgt = std::string("pLights[" + std::to_string(i) + "]");
			shaderBox.setVec3(tgt +".position", lightPositions[i]);
			shaderBox.setVec3(tgt + ".ambient", 0.05f, 0.05f, 0.05f);
			shaderBox.setVec3(tgt + ".diffuse", 0.5f, 0.5f, 0.5f);
			shaderBox.setVec3(tgt + ".specular", 1.0f, 1.0f, 1.0f);
			shaderBox.setFloat(tgt + ".constant", 1.0f);
			shaderBox.setFloat(tgt + ".linear", 0.045f);
			shaderBox.setFloat(tgt + ".quadratic", 0.0075f);
		}

		//shaderBox.setVec3("pLights[0].position", lightPositions[0]);
		//shaderBox.setVec3("pLights[0].ambient", 0.05f, 0.05f, 0.05f);
		//shaderBox.setVec3("pLights[0].diffuse", 0.5f, 0.5f, 0.5f);
		//shaderBox.setVec3("pLights[0].specular", 1.0f, 1.0f, 1.0f);
		//shaderBox.setFloat("pLights[0].constant", 1.0f);
		//shaderBox.setFloat("pLights[0].linear", 0.045f);
		//shaderBox.setFloat("pLights[0].quadratic", 0.0075f);

		/*shaderBox.setVec3("pLights[1].position", lightPositions[1]);
		shaderBox.setVec3("pLights[1].ambient", 0.2f, 0.2f, 0.2f);
		shaderBox.setVec3("pLights[1].diffuse", 0.5f, 0.5f, 0.5f);
		shaderBox.setVec3("pLights[1].specular", 1.0f, 1.0f, 1.0f);
		shaderBox.setFloat("pLights[1].constant", 1.0f);
		shaderBox.setFloat("pLights[1].linear", 0.045f);
		shaderBox.setFloat("pLights[1].quadratic", 0.0075f);

		shaderBox.setVec3("pLights[2].position", lightPositions[2]);
		shaderBox.setVec3("pLights[2].ambient", 0.2f, 0.2f, 0.2f);
		shaderBox.setVec3("pLights[2].diffuse", 0.5f, 0.5f, 0.5f);
		shaderBox.setVec3("pLights[2].specular", 1.0f, 1.0f, 1.0f);
		shaderBox.setFloat("pLights[2].constant", 1.0f);
		shaderBox.setFloat("pLights[2].linear", 0.045f);
		shaderBox.setFloat("pLights[2].quadratic", 0.0075f);

		shaderBox.setVec3("pLights[3].position", lightPositions[3]);
		shaderBox.setVec3("pLights[3].ambient", 0.2f, 0.2f, 0.2f);
		shaderBox.setVec3("pLights[3].diffuse", 0.2f, 0.2f, 0.2f);
		shaderBox.setVec3("pLights[3].specular", 1.0f, 1.0f, 1.0f);
		shaderBox.setFloat("pLights[3].constant", 1.0f);
		shaderBox.setFloat("pLights[3].linear", 0.045f);
		shaderBox.setFloat("pLights[3].quadratic", 0.0075f);*/
		
		shaderBox.setVec3("sLight.position", cam.getCamPos());
		shaderBox.setVec3("sLight.direction", cam.getCamFront());
		shaderBox.setVec3("sLight.ambient", 0.2f, 0.2f, 0.2f);
		shaderBox.setVec3("sLight.diffuse", 0.5f, 0.5f, 0.5f);
		shaderBox.setVec3("sLight.specular", 1.0f, 1.0f, 1.0f);
		shaderBox.setFloat("sLight.cutoff", glm::cos(glm::radians(12.5f)));
		shaderBox.setFloat("sLight.outerCutoff", glm::cos(glm::radians(17.5f)));
		shaderBox.setFloat("sLight.constant", 1.0f);
		shaderBox.setFloat("sLight.linear", 0.045f);
		shaderBox.setFloat("sLight.quadratic", 0.0075f);


		shaderBox.setVec3("viewPos", cam.getCamPos());
		shaderBox.setMat4("view", view);
		shaderBox.setMat4("project", project);
		glBindVertexArray(cubeVao);
		for (int i = 0; i < std::size(cubePositions); i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
			shaderBox.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
#pragma endregion

#pragma region Clear
	glDeleteVertexArrays(1, &cubeVao);
	glDeleteVertexArrays(1, &lightVao);
	glDeleteBuffers(1, &vbo);
	glfwTerminate();
#pragma endregion
	return 0;
}

