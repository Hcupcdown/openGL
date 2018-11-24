#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
	//将模型从世界转换为以光源为摄像机视角的坐标系
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);
}