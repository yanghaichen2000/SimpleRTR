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


// 随机数生成器，范围[0, 1)
// x: 任意数
highp float rand_1to1(highp float x) {
    return fract(sin(x)*10000.0);
}


// 随机数生成器，范围[0, 1)
// uv: 任意二维向量
highp float rand_2to1(vec2 uv) {
    const highp float a = 12.9898, b = 78.233, c = 43758.5453;
    highp float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, PI);
    return fract(sin(sn) * c);
}


// 生成滤波核坐标偏移，正方形采样
// size: kernel的长度和宽度
// range: 每个维度的采样点个数
// index: 当前采样点的序号，可以是[0, range ^ 2 - 1]
vec2 delta_uv_square(float size, int range, int index) {
    int index_u = index % range;
    int index_v = index / range;

    float size_mul_range_inv = size / float(range);
    float delta_u = (float(index_u) - 0.5f * float(range) + 0.5f) * size_mul_range_inv;
    float delta_v = (float(index_v) - 0.5f * float(range) + 0.5f) * size_mul_range_inv;

    return vec2(delta_u, delta_v);
}


// 生成滤波核坐标偏移，泊松圆盘采样
// TODO
vec2 delta_uv_poison() {
    return vec2(0.0f);
}


// 获取该fragment被光源照亮的程度
// 计算方法：使用percentage closer filtering对深度比较的结果进行模糊
// wi: 入射光方向，用于动态调整深度图偏移量
// normal: fragment的法向，用于动态调整深度图偏移量
float get_direct_light_weight(vec3 wi, vec3 normal, float size, int range) {
    
    // 计算该fragment在光源空间的坐标
    vec4 coord_light = projection_light * view_light * model_light * vec4(FragPos, 1.0f);
    
    // 计算该fragment在深度图的uv 
    vec2 uv_light = vec2(coord_light.x / coord_light.w, coord_light.y / coord_light.w);
    // 这里进行一些坐标变换的原因是opengl的裁剪空间为[-1, 1] * [-1, 1] * [-1, 1]，
    // 但是uv坐标的范围是[0, 1] * [0, 1]
    uv_light = (uv_light + 1.0f) * 0.5f; 

    // 计算该fragment在光源空间的深度
    // 同样地，将深度从[-1, 1]映射到[0, 1]
    float depth_light = (coord_light.z / coord_light.w + 1.0f) * 0.5f;

    // 深度图偏移
    float depth_bias = 0.01f;
    depth_bias *= 1 - abs(dot(wi, normal));
    
    // 在深度图上做PCF
    float total_sample_weight = 0.0f;
    float total_sample_value = 0.0f;
    for (int i = 0; i < range * range; i++) {
        vec2 sample_delta_uv = delta_uv_square(size, range, i);
        vec2 sample_uv_light = uv_light + sample_delta_uv;
        float sample_weight = exp(-dot(sample_delta_uv, sample_delta_uv));
        
        // 根据fragment在光源空间的深度和depth map深度确定阴影
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
    //vec3 wi = normalize(lightPos - FragPos); // 点光
    vec3 wi = normalize(lightDir); // 平行光
    vec3 wo = normalize(viewPos - FragPos);
    vec3 h = normalize(wi + wo);
    vec3 diffuse_color = 0.5f * vec3(pow(texture(color_map_0, UV).x, 2.2f), pow(texture(color_map_0, UV).y, 2.2f), pow(texture(color_map_0, UV).z, 2.2f));
    float distance_light_inv2 = 1.0f / dot(lightPos - FragPos, lightPos - FragPos);
    
    // 环境光
    vec3 ambient = 0.02f * diffuse_color;
    
    // 漫反射项
    vec3 diffuse = diffuse_color * lightColor * dot(normal, wi) * distance_light_inv2;
    diffuse = clamp(diffuse, 0.0f, 1.0f);

    // 高光项
    vec3 specular = 0.5f * lightColor * pow(clamp(dot(normal, h), 0.0, 1.0), 32.0) * distance_light_inv2;

    // 获取直接光系数
    float direct_light_weight = get_direct_light_weight(wi, normal, 0.005, 6);

    // 计算fragment颜色
    color = vec4(ambient + direct_light_weight * (diffuse + specular), 1.0f);

    // gamma校正
    color = vec4(pow(color.x, 1.0f / 2.2f), pow(color.y, 1.0f / 2.2f), pow(color.z, 1.0f / 2.2f), 1.0f);

    //color = vec4(vec3(depth_light + 1.0f) * 0.5f, 1.0f);
    //color = vec4(vec3(texture(shadow_map_0, uv_light).r), 1.0f);
}