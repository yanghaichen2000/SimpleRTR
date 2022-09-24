#version 330 core
out vec4 color;

uniform vec3 objectColor;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightPos;

uniform sampler2D tex1;

in vec3 Normal;
in vec2 UV;
in vec3 FragPos;


void main()
{
    vec3 normal = normalize(Normal);
    vec3 wi = normalize(lightPos - FragPos);
    vec3 wo = normalize(viewPos - FragPos);
    vec3 h = normalize(wi + wo);
    vec3 diffuse_color = vec3(pow(texture(tex1, UV).x, 2.2f), pow(texture(tex1, UV).y, 2.2f), pow(texture(tex1, UV).z, 2.2f));
    float distance_light_inv2 = 1.0f / dot(lightPos - FragPos, lightPos - FragPos);
    
    // 环境光
    vec3 ambient = 0.02f * diffuse_color;
    
    // 漫反射项
    vec3 diffuse = diffuse_color * lightColor * dot(normal, wi) * distance_light_inv2;
    diffuse = clamp(diffuse, 0.0f, 1.0f);

    // 高光项
    vec3 specular = 0.5f * lightColor * pow(clamp(dot(normal, h), 0.0, 1.0), 32.0) * distance_light_inv2;

    color = vec4(ambient + diffuse + specular, 1.0f);
    color = vec4(pow(color.x, 1.0f / 2.2f), pow(color.y, 1.0f / 2.2f), pow(color.z, 1.0f / 2.2f), 1.0f);
}