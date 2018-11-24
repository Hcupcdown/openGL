#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
	//�Ե�ǰ�����Ϊ�ӽǵ�����
    FragPos = vec3(model * vec4(position, 1.0));
    Normal = transpose(inverse(mat3(model))) * normal;
    TexCoords = texCoords;
	//��Ӧ����ԴΪ�ӽǵ�����
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}