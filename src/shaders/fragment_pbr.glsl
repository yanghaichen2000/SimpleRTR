#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform sampler2D color_map_0;
uniform sampler2D normal_map_0;
uniform sampler2D metallic_map_0;
uniform sampler2D roughness_map_0;
uniform sampler2D ao_map_0;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// light
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightDir;

// light MVP
uniform mat4 model_light;
uniform mat4 view_light;
uniform mat4 projection_light;

// shadow map
uniform sampler2D shadow_map_0;

uniform vec3 camPos;

const float PI = 3.14159265359;
const float PI_MUL_2 = 6.283185307179586;
const float PI_INV = 0.318309886;



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
// size: kernel的最大半径
// range: 采样点个数的平方根
// index: 当前采样点的序号，可以是[0, range ^ 2 - 1]
const float num_rings = 2.0f;
vec2 delta_uv_poison(float size, int range, int index) {
    float num = range * range;
    float radius = size / num * index * 0.5;
    float angle = PI_MUL_2 * num_rings / num * index;
    
    return vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
}


// 获取该fragment被光源照亮的程度
// 计算方法：使用percentage closer filtering对深度比较的结果进行模糊
// wi: 入射光方向，用于动态调整深度图偏移量
// normal: fragment的法向，用于动态调整深度图偏移量
float get_direct_light_weight(vec3 wi, vec3 normal, float size, int range) {
    
    // 计算该fragment在光源空间的坐标
    vec4 coord_light = projection_light * view_light * model_light * vec4(WorldPos, 1.0f);
    
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
        vec2 sample_delta_uv = delta_uv_poison(size, range, i);
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


// 根据法线贴图计算法线
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normal_map_0, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


// 计算GGX分布NDF
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}


// 计算GGX几何项（单边）
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}


// 计算GGX几何项
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}


// 计算近似菲涅尔项
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


// 计算粗糙度矫正的菲涅尔项
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   



void main()
{		
    // 根据贴图得到属性信息
    vec3 albedo = pow(texture(color_map_0, TexCoords).rgb, vec3(2.2));
    float metallic = texture(metallic_map_0, TexCoords).r;
    float roughness = texture(roughness_map_0, TexCoords).r;
    float ao = texture(ao_map_0, TexCoords).r;


    // 计算法线、反射方向、和镜面反射入射方向
    // 法线根据真实法线和法线贴图得到
    vec3 N = getNormalFromMap();
    // 视线方向
    vec3 V = normalize(camPos - WorldPos);
    // 镜面反射的入射方向
    vec3 R = reflect(-V, N); 


    // 计算菲涅尔项中的F0
    // 对于非金属材质，认为F0=0.04
    // 对于金属材质，F0即等于其发射率，也就是颜色
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);


    // ---------------------------------------------------------------------
    // 计算直接光照

    vec3 Lo = vec3(0.0);

    // 计算光源的radiance
    // 这里是使用的是平行光
    vec3 L = normalize(lightDir);
    vec3 H = normalize(V + L);
    vec3 radiance = lightColor;

    // 计算brdf中的DGF项
    float NDF = DistributionGGX(N, H, roughness);   
    float G = GeometrySmith(N, V, L, roughness);    
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);        
    
    // 计算brdf
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
    
    // specular权重等于菲涅尔项
    vec3 kS = F;
    // diffuse和specular权重之和为1
    vec3 kD = vec3(1.0) - kS;
    // 由于金属材质不具有diffuse成分，所以将diffuse权重乘以(1-metallic)
    kD *= 1.0 - metallic;	                
            
    // 计算cos项
    float NdotL = max(dot(N, L), 0.0);        

    // 得到最终反射radiance
    Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    

    // ---------------------------------------------------------------------
    // 计算阴影

    // 获取直接光系数
    float direct_light_weight = get_direct_light_weight(V , Normal, 0.005, 6);

    Lo *= direct_light_weight;
    

    // ---------------------------------------------------------------------
    // 计算环境光照

    // ibl的菲涅尔项
    vec3 F_ibl = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    // 这一部分与之前计算直接光照相同
    vec3 kS_ibl = F_ibl;
    vec3 kD_ibl = 1.0 - kS_ibl;
    kD_ibl *= 1.0 - metallic;	  
    
    // 根据irradiance map计算diffuse环境光照
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse_ibl = irradiance * albedo;

    // 根据粗糙度在预计算环境贴图上进行采样
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;
    
    // 根据粗糙度和反射方向获取brdf值
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular_ibl = prefilteredColor * (F * brdf.x + brdf.y);

    // 得到最终环境光照
    vec3 ambient = (kD * diffuse_ibl + specular_ibl) * ao;
    

    // ---------------------------------------------------------------------
    
    // 最终结果=环境光+直接光
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    

    FragColor = vec4(color, 1.0);
    //FragColor = vec4(vec3(roughness), 1.0f);
}
