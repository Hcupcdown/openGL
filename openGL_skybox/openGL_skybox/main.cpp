#include <iostream>

#define GLEW_STATIC
#include "include/GLEW/glew.h"
#include "include/GLFW/glfw3.h"

#include "SOIL2/SOIL2.h"
#include "SOIL2/stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "class/Shader.h"
#include "class/Camera.h"
#include "class/Model.h"
#include "class/Mesh.h"
#include "class/Light.h"
#include "class/Skybox.h"

#include <vector>
const GLint WIDTH = 1920, HEIGHT = 1080;
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void ScrollCallback(GLFWwindow *window, double xOffset, double yOffset);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();

//设定摄像机位置
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
bool firstMouse = true;
int main()
{
	//初始化
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "B16040530", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (nullptr == window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//天空盒
	// Set the required callback function
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialise GLEW" << std::endl;
		return -1;
	}
	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//生成三个着色器
	//模型着色器
	Shader shader("res/shaders/modelLoad.vs", "res/shaders/modelLoad.frag");
	//光源的着色器
	Shader lightShader("res/shaders/core.vs", "res/shaders/core.frag");
	//天空盒着色器
	Shader skyboxShader("res/shaders/sky_core.vs", "res/shaders/sky_core.frag");
	// 加载模型
	char c[] = "res/models/nanosuit.obj";
	Model ourModel(c);
	vector<GLchar*> faces;
	//加载天空盒的六个面
	char c0[] = "res/skybox/iceflats_rt.tga";
	char c1[] = "res/skybox/iceflats_lf.tga";
	char c2[] = "res/skybox/iceflats_up.tga";
	char c3[] = "res/skybox/iceflats_dn.tga";
	char c4[] = "res/skybox/iceflats_bk.tga";
	char c5[] = "res/skybox/iceflats_ft.tga";
	faces.push_back(c0);
	faces.push_back(c1);
	faces.push_back(c2);
	faces.push_back(c3);
	faces.push_back(c4);
	faces.push_back(c5);
	//生成光源模型
	Light lightModel = Light();
	//生成天空盒模型
	Skybox skybox = Skybox(faces);
	//三个光照源位置
	glm::vec3 LightPos0 = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 LightPos1 = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 LightPos2 = glm::vec3(0.0f, 0.0f, 1.0f);
	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();
		DoMovement();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;
		//关闭深度测试
		glDepthMask(GL_FALSE);
		//绘制天空盒
		skyboxShader.Use();
		//生成视图矩阵
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		//生成投影矩阵
		glm::mat4 projection = glm::perspective(1.0f, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);
		//绘制天空盒
		skybox.Draw(skyboxShader);
		//开启深度测试
		glDepthMask(GL_TRUE);
		//旋转三个光照源
		//三个参数为，原矩阵，旋转角度，旋转轴
		LightPos0 = glm::rotate(LightPos0, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
		LightPos1 = glm::rotate(LightPos1, 0.01f, glm::vec3(0.0f, 0.0f, 1.0f));
		LightPos2 = glm::rotate(LightPos2, 0.01f, glm::vec3(1.0f, 0.0f, 0.0f));
		//模型矩阵
		glm::mat4 model = glm::mat4(0.4f);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.GetZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		//使用光照着色器
		lightShader.Use();
		//光源的模型矩阵
		model = glm::translate(model, LightPos0);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(lightShader.Program, "inColor"), 1.0f, 0.0f, .0f);
		lightModel.Draw(lightShader);
		//第二个光源
		model = glm::mat4(1.0f);
		model = glm::translate(model, LightPos1);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(lightShader.Program, "inColor"), 0.0f, 1.0f, 0.0f);
		lightModel.Draw(lightShader);
		//第三个光源
		model = glm::mat4(1.0f);
		model = glm::translate(model, LightPos2);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(lightShader.Program, "inColor"), 0.0f, 0.0f, 1.0f);
		lightModel.Draw(lightShader);
		//绘制模型
		shader.Use();
		glm::mat4 model = glm::mat4(1.0f);
		model = translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, (GLfloat)glfwGetTime()*glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//传入摄像机位置
		glUniform3f(glGetUniformLocation(shader.Program, "ViewPos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 64.0f);
		//传入三个光源位置
		glUniform3f(glGetUniformLocation(shader.Program, "LightPos0"), LightPos0.x, LightPos0.y, LightPos0.z);
		glUniform3f(glGetUniformLocation(shader.Program, "LightPos1"), LightPos1.x, LightPos1.y, LightPos1.z);
		glUniform3f(glGetUniformLocation(shader.Program, "LightPos2"), LightPos2.x, LightPos2.y, LightPos2.z);
		//传入三个光源颜色
		glUniform3f(glGetUniformLocation(shader.Program, "LightColor0"), 1.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader.Program, "LightColor1"), 0.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader.Program, "LightColor2"), 0.0f, 0.0f, 1.0f);
		//绘制模型
		ourModel.Draw(shader);
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS) {
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
}
void ScrollCallback(GLFWwindow *window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(yOffset);
}
void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}
	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	// Process the camera direction
	camera.ProcessMouseMovement(xOffset, yOffset);
}
void DoMovement()
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

}
