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
		//输出出错信息

		vShaderFile.exceptions(ifstream::badbit);
		fShaderFile.exceptions(ifstream::badbit);
		try {
			//打开文件
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			stringstream vShaderStream, fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			vShaderFile.close();
			fShaderFile.close();
			//顶点着色器代码
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (ifstream::failure e) {
			cout << "你代码打错啦！！！！" << endl;
		}
		const GLchar *vShaderCode = vertexCode.c_str();
		const GLchar *fShaderCode = fragmentCode.c_str();
		//生成顶点着色器
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		//装载着色器
		glShaderSource(vertex, 1,&vShaderCode , NULL);
		//编译
		glCompileShader(vertex);
		GLint success;
		GLchar infoLog[512];
		//获取编译状态
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		//装载着色器
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		//编译
		glCompileShader(fragment);
		//获取编译状态
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		//创建程序并链接
		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		//调用链接器
		glLinkProgram(this->Program);
		//测试链接是否成功
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
		}
		glValidateProgram(this->Program);
		//检测program能否运行
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success) 
		{


		}
	}
	~Shader()
	{
		glDetachShader(this->Program, vertex);
		glDetachShader(this->Program, fragment);
		glDeleteShader(vertex);//清除着色器
		glDeleteShader(fragment);
		glDeleteShader(this->Program);
	}
	void Use()
	{
		glUseProgram(this->Program);
	}

};