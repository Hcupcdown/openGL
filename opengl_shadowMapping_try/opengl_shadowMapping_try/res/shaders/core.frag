#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform sampler2D shadowMap;
uniform sampler2D diffuseTexture;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    //
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    //将深度转换到[0,1]区间
    projCoords = projCoords * 0.5 + 0.5;

    //对这一个区域内的深度贴图采样
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    //当前片源的深度
    float currentDepth = projCoords.z;


    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

	//将片源上移，解决阴影失真问题
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;

	//单个像素大小,textureSize返回一个给定采样器纹理的0级mipmap的vec2类型的宽和高
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

	//PCF对周围的深度图采样，平均化，使阴影更柔和
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
	shadow /= 9.0;
	
    // 将光照不到的地方置为0
    if(projCoords.z >= 1.0f)
     shadow = 0.0f;
        
    return shadow;
}

void main()
{           
    vec3 color = texture(diffuseTexture, TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0f);

    //环境光
    vec3 ambient = 0.2 * color;

    //漫反射
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.3);
    vec3 diffuse = diff * lightColor;

    //镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.3), 64.0);
    vec3 specular = spec * lightColor;    
	//判断点是否在阴影里面
    float shadow = ShadowCalculation(FragPosLightSpace);     

    vec3 lighting = (ambient + (1.0-shadow)*(diffuse + specular)) * color;    

    FragColor = vec4(lighting, 1.0f);
}