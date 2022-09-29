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

#define PI 3.141592653589793
#define PI_MUL_2 6.283185307179586


// ���������������Χ[0, 1)
// x: ������
highp float rand_1to1(highp float x) {
    return fract(sin(x)*10000.0);
}


// ���������������Χ[0, 1)
// uv: �����ά����
highp float rand_2to1(vec2 uv) {
    const highp float a = 12.9898, b = 78.233, c = 43758.5453;
    highp float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, PI);
    return fract(sin(sn) * c);
}


// �����˲�������ƫ�ƣ������β���
// size: kernel�ĳ��ȺͿ��
// range: ÿ��ά�ȵĲ��������
// index: ��ǰ���������ţ�������[0, range ^ 2 - 1]
vec2 delta_uv_square(float size, int range, int index) {
    int index_u = index % range;
    int index_v = index / range;

    float size_mul_range_inv = size / float(range);
    float delta_u = (float(index_u) - 0.5f * float(range) + 0.5f) * size_mul_range_inv;
    float delta_v = (float(index_v) - 0.5f * float(range) + 0.5f) * size_mul_range_inv;

    return vec2(delta_u, delta_v);
}


// �����˲�������ƫ�ƣ�����Բ�̲���
// TODO
vec2 delta_uv_poison() {
    return vec2(0.0f);
}


// ��ȡ��fragment����Դ�����ĳ̶�
// ���㷽����ʹ��percentage closer filtering����ȱȽϵĽ������ģ��
// wi: ����ⷽ�����ڶ�̬�������ͼƫ����
// normal: fragment�ķ������ڶ�̬�������ͼƫ����
float get_direct_light_weight(vec3 wi, vec3 normal, float size, int range) {
    
    // �����fragment�ڹ�Դ�ռ������
    vec4 coord_light = projection_light * view_light * model_light * vec4(FragPos, 1.0f);
    
    // �����fragment�����ͼ��uv 
    vec2 uv_light = vec2(coord_light.x / coord_light.w, coord_light.y / coord_light.w);
    // �������һЩ����任��ԭ����opengl�Ĳü��ռ�Ϊ[-1, 1] * [-1, 1] * [-1, 1]��
    // ����uv����ķ�Χ��[0, 1] * [0, 1]
    uv_light = (uv_light + 1.0f) * 0.5f; 

    // �����fragment�ڹ�Դ�ռ�����
    // ͬ���أ�����ȴ�[-1, 1]ӳ�䵽[0, 1]
    float depth_light = (coord_light.z / coord_light.w + 1.0f) * 0.5f;

    // ���ͼƫ��
    float depth_bias = 0.01f;
    depth_bias *= 1 - abs(dot(wi, normal));
    
    // �����ͼ����PCF
    float total_sample_weight = 0.0f;
    float total_sample_value = 0.0f;
    for (int i = 0; i < range * range; i++) {
        vec2 sample_delta_uv = delta_uv_square(size, range, i);
        vec2 sample_uv_light = uv_light + sample_delta_uv;
        float sample_weight = exp(-dot(sample_delta_uv, sample_delta_uv));
        
        // ����fragment�ڹ�Դ�ռ����Ⱥ�depth map���ȷ����Ӱ
        float sample_value;
        if (texture(shadow_map_0, sample_uv_light).x + depth_bias > depth_light) {
            sample_value = 1.0f;
        }
        else {
            sample_value = 0.0f;
        }

        total_sample_weight += sample_weight;
        total_sample_value += sample_weight * sample_value;
    }

    return total_sample_value / total_sample_weight;
}


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

    // ��ȡֱ�ӹ�ϵ��
    float direct_light_weight = get_direct_light_weight(wi, normal, 0.005, 6);

    // ����fragment��ɫ
    color = vec4(ambient + direct_light_weight * (diffuse + specular), 1.0f);

    // gammaУ��
    color = vec4(pow(color.x, 1.0f / 2.2f), pow(color.y, 1.0f / 2.2f), pow(color.z, 1.0f / 2.2f), 1.0f);

    //color = vec4(vec3(depth_light + 1.0f) * 0.5f, 1.0f);
    //color = vec4(vec3(texture(shadow_map_0, uv_light).r), 1.0f);
}