#include <iostream>
#define GLEW_STATIC
#include "include/GLEW/glew.h"
#include "include/GLFW/glfw3.h"

#include "include/SOIL2/SOIL2.h"
#include "include/SOIL2/stb_image.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtx/rotate_vector.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "class/Shader.h"
#include "class/Camera.h"
#include "class/Model.h"
#include "class/Mesh.h"
#include "class/Light.h"

#include <vector>
const GLint WIDTH = 1920, HEIGHT = 1080;
//键盘事件回调
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
//滚轮事件回调
void ScrollCallback(GLFWwindow *window, double xOffset, double yOffset);
//鼠标事件回调
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
//纹理路径
GLuint loadCubemap(vector<GLchar*> faces);

//设定摄像机位置
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
//两帧间隔时间
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
//记录回放数组大小
int replaySize;
//记录键值，按下时间,结束时间，用于回放
vector<int> replaykey;
vector<float> replayBTime;
vector<float> replayETime;
//开始时间
int beginTime0,beginTime1;
//是否是回放状态
bool rFlag = false;
bool firstMouse = true;
int main()
{
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
	// 绑定回调事件
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
	//创建着色器
	Shader shader("res/shaders/modelLoad.vs", "res/shaders/modelLoad.frag");
	Shader lightShader("res/shaders/core.vs", "res/shaders/core.frag");
	// 载入模型
	char c[] = "res/models/nanosuit.obj";
	Model ourModel(c);

	//载入光源
	Light lightModel0 = Light();
	//三个光照源位置
	glm::vec3 LightPos0 = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 LightPos1 = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 LightPos2 = glm::vec3(0.0f, 0.0f, 1.0f);
	int i = 0;
	//记录开始时间
	beginTime0 = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		//如果是回放状态
		if (rFlag)
		{
			
			if (fabs(glfwGetTime() - beginTime1 - (replayBTime[i]))<0.01)
			{
				keys[replaykey[i]] = true;
			}
			if (fabs(glfwGetTime() - beginTime1 - replayETime[i])<0.01)
			{
				keys[replaykey[i]] = false;
				i++;
			}
			if (i >= replaySize)
			{
				rFlag = false;
				replaykey.clear();
				replayBTime.clear();
				replayETime.clear();
			}
		}
		DoMovement();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;
		//开启深度测试
		glDepthMask(GL_TRUE);
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glm::mat4 projection = glm::perspective(1.0f, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		
		//旋转三个光照源
		//三个参数为，原矩阵，旋转角度，旋转轴
		LightPos0 = glm::rotate(LightPos0, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
		LightPos1 = glm::rotate(LightPos1, 0.01f, glm::vec3(0.0f, 0.0f, 1.0f));
		LightPos2 = glm::rotate(LightPos2, 0.01f, glm::vec3(1.0f, 0.0f, 0.0f));
		//模型矩阵
		glm::mat4 model0 = glm::mat4(0.4f);
		//使用光照着色器
		lightShader.Use();
		//获取摄像机位置、朝向信息
		view = camera.GetViewMatrix();
		//第一个参数为可视角度，第二个参数为长宽比，第三个参数为近截面，第四个为远截面
		projection = glm::perspective(glm::radians(camera.GetZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		model0 = glm::translate(model0, LightPos0);
		//缩放
		model0 = glm::scale(model0, glm::vec3(0.1f, 0.1f, 0.1f));
		model0 = translate(model0, glm::vec3(0.0f, 0.0f, 0.0f));
		//传递矩阵信息
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model0));
		//传入颜色
		glUniform3f(glGetUniformLocation(lightShader.Program, "inColor"), 1.0f, 0.0f, .0f);
		//绘制立方体
		lightModel0.Draw(lightShader);
		glm::mat4 model1 = glm::mat4(1.0f);
		model1 = glm::translate(model1, LightPos1);
		model1 = glm::scale(model1, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model1));

		glUniform3f(glGetUniformLocation(lightShader.Program, "inColor"), 0.0f, 1.0f, 0.0f);
		lightModel0.Draw(lightShader);
		glm::mat4 model2 = glm::mat4(1.0f);
		model2 = glm::translate(model2, LightPos2);
		model2 = glm::scale(model2, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model2));
		glUniform3f(glGetUniformLocation(lightShader.Program, "inColor"), 0.0f, 0.0f, 1.0f);
		lightModel0.Draw(lightShader);
		//使用着色器
		shader.Use();
		glm::mat4 model = glm::mat4(1.0f);
		//转换
		model = translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//让模型旋转
		model = glm::rotate(model, (GLfloat)glfwGetTime()*glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//传入视图矩阵，模型矩阵
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(shader.Program, "ViewPos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(shader.Program, "LightPos0"), LightPos0.x, LightPos0.y, LightPos0.z);
		glUniform3f(glGetUniformLocation(shader.Program, "LightPos1"), LightPos1.x, LightPos1.y, LightPos1.z);
		glUniform3f(glGetUniformLocation(shader.Program, "LightPos2"), LightPos2.x, LightPos2.y, LightPos2.z);
		glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 64.0f);
		glUniform3f(glGetUniformLocation(shader.Program, "LightColor0"), 1.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader.Program, "LightColor1"), 0.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader.Program, "LightColor2"), 0.0f, 0.0f, 1.0f);
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
	//如果按下R键
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		//将回放标识置为true
		rFlag = true;
		//将摄像机返回到原始位置
		camera.Change(glm::vec3(0.0f, 0.0f, 2.0f));
		//获取回放按键容器大小
		replaySize = replaykey.size();
		//记录下现在时间
		beginTime1 = glfwGetTime();
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS) {
			
			//记录下按下的键
			if (!rFlag)
			{
				keys[key] = true;
				replaykey.push_back(key);
				//记录下按下时间
				replayBTime.push_back(glfwGetTime() - beginTime0);
			}
			
		}
		else if (action == GLFW_RELEASE)
		{
			
			if (!rFlag)
			{
				keys[key] = false;
				//记录下松开时间
				replayETime.push_back(glfwGetTime() - beginTime0);
			}
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
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

}
GLuint loadCubemap(vector<GLchar*> faces)
{

	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);    //  // 释放源图像内存
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}





