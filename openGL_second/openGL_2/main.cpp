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
//�����¼��ص�
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
//�����¼��ص�
void ScrollCallback(GLFWwindow *window, double xOffset, double yOffset);
//����¼��ص�
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
//����·��
GLuint loadCubemap(vector<GLchar*> faces);

//�趨�����λ��
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
//��֡���ʱ��
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
//��¼�ط������С
int replaySize;
//��¼��ֵ������ʱ��,����ʱ�䣬���ڻط�
vector<int> replaykey;
vector<float> replayBTime;
vector<float> replayETime;
//��ʼʱ��
int beginTime0,beginTime1;
//�Ƿ��ǻط�״̬
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
	// �󶨻ص��¼�
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
	//������ɫ��
	Shader shader("res/shaders/modelLoad.vs", "res/shaders/modelLoad.frag");
	Shader lightShader("res/shaders/core.vs", "res/shaders/core.frag");
	// ����ģ��
	char c[] = "res/models/nanosuit.obj";
	Model ourModel(c);

	//�����Դ
	Light lightModel0 = Light();
	//��������Դλ��
	glm::vec3 LightPos0 = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 LightPos1 = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 LightPos2 = glm::vec3(0.0f, 0.0f, 1.0f);
	int i = 0;
	//��¼��ʼʱ��
	beginTime0 = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		//����ǻط�״̬
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
		//������Ȳ���
		glDepthMask(GL_TRUE);
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glm::mat4 projection = glm::perspective(1.0f, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		
		//��ת��������Դ
		//��������Ϊ��ԭ������ת�Ƕȣ���ת��
		LightPos0 = glm::rotate(LightPos0, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
		LightPos1 = glm::rotate(LightPos1, 0.01f, glm::vec3(0.0f, 0.0f, 1.0f));
		LightPos2 = glm::rotate(LightPos2, 0.01f, glm::vec3(1.0f, 0.0f, 0.0f));
		//ģ�;���
		glm::mat4 model0 = glm::mat4(0.4f);
		//ʹ�ù�����ɫ��
		lightShader.Use();
		//��ȡ�����λ�á�������Ϣ
		view = camera.GetViewMatrix();
		//��һ������Ϊ���ӽǶȣ��ڶ�������Ϊ����ȣ�����������Ϊ�����棬���ĸ�ΪԶ����
		projection = glm::perspective(glm::radians(camera.GetZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		model0 = glm::translate(model0, LightPos0);
		//����
		model0 = glm::scale(model0, glm::vec3(0.1f, 0.1f, 0.1f));
		model0 = translate(model0, glm::vec3(0.0f, 0.0f, 0.0f));
		//���ݾ�����Ϣ
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model0));
		//������ɫ
		glUniform3f(glGetUniformLocation(lightShader.Program, "inColor"), 1.0f, 0.0f, .0f);
		//����������
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
		//ʹ����ɫ��
		shader.Use();
		glm::mat4 model = glm::mat4(1.0f);
		//ת��
		model = translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//��ģ����ת
		model = glm::rotate(model, (GLfloat)glfwGetTime()*glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//������ͼ����ģ�;���
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
	//�������R��
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		//���طű�ʶ��Ϊtrue
		rFlag = true;
		//����������ص�ԭʼλ��
		camera.Change(glm::vec3(0.0f, 0.0f, 2.0f));
		//��ȡ�طŰ���������С
		replaySize = replaykey.size();
		//��¼������ʱ��
		beginTime1 = glfwGetTime();
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS) {
			
			//��¼�°��µļ�
			if (!rFlag)
			{
				keys[key] = true;
				replaykey.push_back(key);
				//��¼�°���ʱ��
				replayBTime.push_back(glfwGetTime() - beginTime0);
			}
			
		}
		else if (action == GLFW_RELEASE)
		{
			
			if (!rFlag)
			{
				keys[key] = false;
				//��¼���ɿ�ʱ��
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
		SOIL_free_image_data(image);    //  // �ͷ�Դͼ���ڴ�
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}





