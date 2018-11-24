#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
	//��ģ�ʹ�����ת��Ϊ�Թ�ԴΪ������ӽǵ�����ϵ
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);
}