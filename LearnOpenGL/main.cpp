#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
	unsigned int emissionMap = loadTexture("textures/container_emission.jpg");
#pragma endregion
	
	glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
	shaderBox.use();
	shaderBox.setInt("material.diffuse", 0);
	shaderBox.setInt("material.specular", 1);
	shaderBox.setInt("material.emission", 2);
#pragma region RenderLoop
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shaderLight.use();
		glm::vec3 lightColor = glm::vec3(1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // 降低影响
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // 很低的影响
		shaderLight.setVec3("lightColor", lightColor);
		glm::mat4 view = cam.getViewMat();
		glm::mat4 project = glm::perspective(glm::radians(cam.getFovY()),
			(float)1920 / (float)1080, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		shaderLight.setMat4("view", view);
		shaderLight.setMat4("project", project);
		shaderLight.setMat4("model", model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emissionMap);
		glBindVertexArray(lightVao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		shaderBox.use();
		shaderBox.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		shaderBox.setFloat("material.shininess", 32.0f);
		shaderBox.setVec3("light.ambient", ambientColor);
		shaderBox.setVec3("light.diffuse", diffuseColor);
		shaderBox.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		shaderBox.setVec3("light.position", lightPos);
		shaderBox.setVec3("viewPos", cam.getCamPos());
		model = glm::mat4(1.0f);
		shaderBox.setMat4("view", view);
		shaderBox.setMat4("project", project);
		shaderBox.setMat4("model", model);
		glBindVertexArray(cubeVao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

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

