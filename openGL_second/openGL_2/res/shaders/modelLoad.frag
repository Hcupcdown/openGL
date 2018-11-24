#version 330 core
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 LightPos0;
uniform vec3 LightPos1;
uniform vec3 LightPos2;
uniform vec3 ViewPos;
uniform vec3 LightColor0;
uniform vec3 LightColor1;
uniform vec3 LightColor2;
struct Material
{
    float shininess;
};
uniform Material material;

void main()
{
   // 环境光
    vec3 ambient = (1.0f,1.0f,1.0f)* texture(texture_diffuse1, TexCoords).rgb;
    
    // diffuse
	//标准化矩阵
    vec3 norm = normalize(Normal);
    vec3 lightDir0 = normalize(LightPos0 - FragPos);
	vec3 lightDir1 = normalize(LightPos1 - FragPos);
	vec3 lightDir2 = normalize(LightPos2 - FragPos);
    float diff0 = 0.6f * max( dot(norm, lightDir0), 0.0f);
	float diff1 = 0.6f * max( dot(norm, lightDir1), 0.0f);
	float diff2 = 0.6f * max( dot(norm, lightDir2), 0.0f);
	//将两个光源产生的效果叠加
    vec3 diffuse = diff0 * texture(texture_diffuse1, TexCoords).rgb*LightColor0+diff1*texture(texture_diffuse1, TexCoords).rgb*LightColor1+diff2*texture(texture_diffuse1, TexCoords).rgb*LightColor2;
    
    // 镜面反射
    vec3 viewDir = normalize(ViewPos - FragPos);
	//计算半角
    vec3 halfAngle0 = normalize(viewDir + lightDir0);
	vec3 halfAngle1 = normalize(viewDir + lightDir1);
	vec3 halfAngle2 = normalize(viewDir + lightDir2);
    float spec0 = 0.9f * pow( max(dot(norm, halfAngle0), 0.0f), material.shininess);
	float spec1 = 0.9f * pow( max(dot(norm, halfAngle1), 0.0f), material.shininess);
	float spec2 = 0.9f * pow( max(dot(norm, halfAngle2), 0.0f), material.shininess);
    vec3 specular = spec0*texture(texture_specular1, TexCoords).rgb*LightColor0+spec1 * texture(texture_specular1, TexCoords).rgb*LightColor1+spec2*texture(texture_specular1, TexCoords).rgb*LightColor2;
    
    color = vec4( ambient + diffuse + specular, 1.0f);
    //color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    
    
    
    
    
    
}
