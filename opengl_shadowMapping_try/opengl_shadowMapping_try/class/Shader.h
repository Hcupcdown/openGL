#pragma once
#ifndef Shader_h
#define Shader_h
#endif // !Shader_h
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GLEW\glew.h>
#include <GLFW\glfw3.h>
using namespace std;
class Shader {
	GLuint vertex, fragment;
public:
	GLuint Program;
	Shader(const GLchar *vertexPath,const GLchar *fragmentPath)
	{
		string vertexCode;
		string fragmentCode;
		ifstream vShaderFile;
		ifstream fShaderFile;
		//���������Ϣ

		vShaderFile.exceptions(ifstream::badbit);
		fShaderFile.exceptions(ifstream::badbit);
		try {
			//���ļ�
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			stringstream vShaderStream, fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			vShaderFile.close();
			fShaderFile.close();
			//������ɫ������
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (ifstream::failure e) {
			cout << "�����������������" << endl;
		}
		const GLchar *vShaderCode = vertexCode.c_str();
		const GLchar *fShaderCode = fragmentCode.c_str();
		//���ɶ�����ɫ��
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		//װ����ɫ��
		glShaderSource(vertex, 1,&vShaderCode , NULL);
		//����
		glCompileShader(vertex);
		GLint success;
		GLchar infoLog[512];
		//��ȡ����״̬
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		//װ����ɫ��
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		//����
		glCompileShader(fragment);
		//��ȡ����״̬
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		//������������
		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		//����������
		glLinkProgram(this->Program);
		//���������Ƿ�ɹ�
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
		}
		glValidateProgram(this->Program);
		//���program�ܷ�����
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success) 
		{


		}
	}
	~Shader()
	{
		glDetachShader(this->Program, vertex);
		glDetachShader(this->Program, fragment);
		glDeleteShader(vertex);//�����ɫ��
		glDeleteShader(fragment);
		glDeleteShader(this->Program);
	}
	void Use()
	{
		glUseProgram(this->Program);
	}

};