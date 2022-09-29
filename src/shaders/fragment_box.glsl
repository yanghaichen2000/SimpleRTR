#version 330 core
out vec4 color;

uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightDir;

uniform mat4 model_light;
uniform mat4 view_light;
uniform mat4 projection_light;

uniform sampler2D color_map_0;
uniform sampler2D shadow_map_0;

in vec3 Normal;
in vec2 UV;
in vec3 FragPos;


void main()
{
    vec3 normal = normalize(Normal);
    //vec3 wi = normalize(lightPos - FragPos); // ���
    vec3 wi = normalize(lightDir); // ƽ�й�
    vec3 wo = normalize(viewPos - FragPos);
    vec3 h = normalize(wi + wo);
    vec3 diffuse_color = 0.5f * vec3(pow(texture(color_map_0, UV).x, 2.2f), pow(texture(color_map_0, UV).y, 2.2f), pow(texture(color_map_0, UV).z, 2.2f));
    float distance_light_inv2 = 1.0f / dot(lightPos - FragPos, lightPos - FragPos);
    
    // ������
    vec3 ambient = 0.02f * diffuse_color;
    
    // ��������
    vec3 diffuse = diffuse_color * lightColor * dot(normal, wi) * distance_light_inv2;
    diffuse = clamp(diffuse, 0.0f, 1.0f);

    // �߹���
    vec3 specular = 0.5f * lightColor * pow(clamp(dot(normal, h), 0.0, 1.0), 32.0) * distance_light_inv2;

    // �����fragment�ڹ�Դ�ռ������
    vec4 coord_light = projection_light * view_light * model_light * vec4(FragPos, 1.0f);
    
    // �����fragment�����ͼ��uv 
    vec2 uv_light = vec2(coord_light.x / coord_light.w, coord_light.y / coord_light.w);
    // �������һЩ�任��ԭ����opengl�Ĳü��ռ�Ϊ[-1, 1] * [-1, 1] * [-1, 1]��
    // ����uv����ķ�Χ��[0, 1] ^ 2
    uv_light = (uv_light + 1.0f) * 0.5f; 

    // �����fragment�ڹ�Դ�ռ�����
    // ͬ���أ�����ȴ�[-1, 1]ӳ�䵽[0, 1]
    float depth_light = (coord_light.z / coord_light.w + 1.0f) * 0.5f;

    // ����fragment�ڹ�Դ�ռ����Ⱥ�depth map���ȷ����Ӱ
    if (texture(shadow_map_0, uv_light).x + 0.002f > depth_light) {
        color = vec4(ambient + diffuse + specular, 1.0f);
    }
    else {
        color = vec4(ambient, 1.0f);
    }

    // gammaУ��
    color = vec4(pow(color.x, 1.0f / 2.2f), pow(color.y, 1.0f / 2.2f), pow(color.z, 1.0f / 2.2f), 1.0f);

    //color = vec4(vec3(depth_light + 1.0f) * 0.5f, 1.0f);
    //color = vec4(vec3(texture(shadow_map_0, uv_light).r), 1.0f);
}