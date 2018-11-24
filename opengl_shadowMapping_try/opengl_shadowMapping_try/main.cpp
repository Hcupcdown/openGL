/*openGL shadow mapping*/
/*create by cup_cdown*/
/*2018-11-18*/
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
#include "class/Skybox.h"
#include "class/Light.h"
#include "class/Plane.h"

//���崰�ڿ��
const GLuint SCR_WIDTH = 1600, SCR_HEIGHT = 1200;

//�������Ļص�����
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

//���������
Camera camera(glm::vec3(0.0f, 3.0f, 3.0f));

//���ڼ���֡���
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//��Դλ��
glm::vec3 lightPos(1.0f, 3.0f, 1.0f);

int main()
{
	//��ʼ������
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "B16040530", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	//���ü������ص��¼�
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	//������Ȳ���
	glEnable(GL_DEPTH_TEST);

	//������ɫ��
	Shader shader("res/shaders/core.vs", "res/shaders/core.frag");
	Shader simpleDepthShader("res/shaders/shadow_mapping_depth.vs", "res/shaders/shadow_mapping_depth.frag");
	Shader skyboxShader("res/shaders/sky_core.vs", "res/shaders/sky_core.frag");
	Shader lightShader("res/shaders/lightcore.vs", "res/shaders/lightcore.frag");

	//���ɵ���
	Plane plane = Plane("res/shaders/image1.jpg");
	
	//������պе�������
	vector<const char*> faces;
	faces.push_back("res/skybox/entropic_rt.tga");
	faces.push_back("res/skybox/entropic_lf.tga");
	faces.push_back("res/skybox/entropic_up.tga");
	faces.push_back("res/skybox/entropic_dn.tga");
	faces.push_back("res/skybox/entropic_bk.tga");
	faces.push_back("res/skybox/entropic_ft.tga");

	//������պ�ģ��
	Skybox skybox = Skybox(faces);

	//���ɹ�Դ
	Light light = Light();

	//��������ģ��
	Model mymodel("res/models/nanosuit.obj");


	//���������ͼ������
	const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT =2048;

	//����һ��2D�����ṩ��֡�������Ȼ���ʹ��
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	//��������SHADOW_WIDTH,SHADOW_HEIGHT��ʾ���������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//����֡�������
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	//�����ɵ����������Ϊ֡�������Ȼ���
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	while (!glfwWindowShouldClose(window))
	{
		//����֡���
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//����Ƿ��м���������
		glfwPollEvents();
		Do_Movement();

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f,20.0f);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//���������굽��ԴΪ�������λ��
		lightSpaceMatrix = lightProjection * lightView;

		//���������Ӱ�������ɫ��
		simpleDepthShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glUniform1f(glGetUniformLocation(simpleDepthShader.Program, "near_plane"), 1.0f);
		glUniform1f(glGetUniformLocation(simpleDepthShader.Program, "far_plane"), 20.0f);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);


		// ���Ƶذ�
		glm::mat4 planeModel;
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(planeModel));
		plane.DrawPlane(simpleDepthShader);

		//����ģ��
		glm::mat4 modelModel;
		modelModel = glm::translate(modelModel, glm::vec3(0.0f, -0.5f, 0.0));
		modelModel = glm::scale(modelModel, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelModel));
		mymodel.Draw(simpleDepthShader);
		//���֡����
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//�����Ӵ���С
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//������պ�
		//�ر���Ȳ���
		glDepthMask(GL_FALSE);
		skyboxShader.Use();
		glm::mat4 skyProjection = glm::perspective(1.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		//���ε�ƽ�Ʋ���
		glm::mat4 skyView = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(skyProjection));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(skyView));
		skybox.Draw(skyboxShader);
		//������Ȳ���
		glDepthMask(GL_TRUE);


		//ʹ�ù�����ɫ��,���ƹ�Դ
		lightShader.Use();
		glm::mat4 lightModel = glm::mat4(0.4f);
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		//��ʱ����Ĺ�Դλ��
		//lightPos = glm::rotate(lightPos, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightModel = glm::translate(lightModel, lightPos);
		lightModel = glm::scale(lightModel, glm::vec3(0.1f, 0.1f, 0.1f));
		lightModel = translate(lightModel, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
		glUniform3f(glGetUniformLocation(lightShader.Program, "inColor"), 1.0f, 1.0f, 1.0f);
		light.Draw(lightShader);

		
		//������ͼ���󣬼��þ��󣬹�Դλ�ã������λ��
		shader.Use();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniform3fv(glGetUniformLocation(shader.Program, "lightPos"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(shader.Program, "viewPos"), 1, glm::value_ptr(camera.GetPosition()));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glUniform1i(glGetUniformLocation(shader.Program, "diffuseTexture"), 0);
		glUniform1i(glGetUniformLocation(shader.Program, "shadowMap"), 1);

		//������
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,plane.returnTextue());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		//���Ƶذ�
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(planeModel));
		plane.DrawPlane(shader);

		//����ģ��
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelModel));
		mymodel.Draw(shader);
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

bool keys[1024];
bool keysPressed[1024];
void Do_Movement()
{
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (keys[GLFW_KEY_SPACE] && !keysPressed[GLFW_KEY_SPACE])
	{
		keysPressed[GLFW_KEY_SPACE] = true;
	}
}

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key <= 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
			keysPressed[key] = false;
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}