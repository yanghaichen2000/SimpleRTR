#version 330 core
out vec4 color;

uniform vec3 objectColor;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightPos;

in vec3 Normal;
in vec3 FragPos;

void main()
{
    vec3 normal = normalize(Normal);
    vec3 wi = normalize(lightPos - FragPos);
    vec3 wo = normalize(viewPos - FragPos);
    vec3 h = normalize(wi + wo);
    
    // 环境光
    vec3 ambient = 0.1f * objectColor * lightColor;
    
    // 漫反射项
    vec3 diffuse = objectColor * lightColor * dot(normal, wi);
    diffuse = clamp(diffuse, 0.0f, 1.0f);

    // 高光项
    vec3 specular = 0.5f * lightColor * pow(clamp(dot(normal, h), 0.0, 1.0), 32.0);

    color = vec4(ambient + diffuse + specular, 1.0f);
}