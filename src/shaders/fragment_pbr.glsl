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
//uniform sampler2D shadow_map_0;

uniform vec3 camPos;

const float PI = 3.14159265359;


// ���ݷ�����ͼ���㷨��
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


// ����GGX�ֲ�NDF
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


// ����GGX��������ߣ�
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}


// ����GGX������
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}


// ������Ʒ�������
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


// ����ֲڶȽ����ķ�������
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   



void main()
{		
    // ������ͼ�õ�������Ϣ
    vec3 albedo = pow(texture(color_map_0, TexCoords).rgb, vec3(2.2));
    float metallic = texture(metallic_map_0, TexCoords).r;
    float roughness = texture(roughness_map_0, TexCoords).r;
    float ao = texture(ao_map_0, TexCoords).r;


    // ���㷨�ߡ����䷽�򡢺;��淴�����䷽��
    // ���߸�����ʵ���ߺͷ�����ͼ�õ�
    vec3 N = getNormalFromMap();
    // ���䷽��
    vec3 V = normalize(camPos - WorldPos);
    // ���淴������䷽��
    vec3 R = reflect(-V, N); 


    // ������������е�F0
    // ���ڷǽ������ʣ���ΪF0=0.04
    // ���ڽ������ʣ�F0�������䷢���ʣ�Ҳ������ɫ
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // ---------------------------------------------------------------------
    // ����ֱ�ӹ���

    vec3 Lo = vec3(0.0);

    // �����Դ��radiance
    // ������ʹ�õ���ƽ�й�
    vec3 L = normalize(lightDir);
    vec3 H = normalize(V + L);
    vec3 radiance = lightColor;

    // ����brdf�е�DGF��
    float NDF = DistributionGGX(N, H, roughness);   
    float G = GeometrySmith(N, V, L, roughness);    
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);        
    
    // ����brdf
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
    
    // specularȨ�ص��ڷ�������
    vec3 kS = F;
    // diffuse��specularȨ��֮��Ϊ1
    vec3 kD = vec3(1.0) - kS;
    // ���ڽ������ʲ�����diffuse�ɷ֣����Խ�diffuseȨ�س���(1-metallic)
    kD *= 1.0 - metallic;	                
            
    // ����cos��
    float NdotL = max(dot(N, L), 0.0);        

    // �õ����շ���radiance
    Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
     
    
    // ---------------------------------------------------------------------
    // ���㻷������

    // ibl�ķ�������
    vec3 F_ibl = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    // ��һ������֮ǰ����ֱ�ӹ�����ͬ
    vec3 kS_ibl = F_ibl;
    vec3 kD_ibl = 1.0 - kS_ibl;
    kD_ibl *= 1.0 - metallic;	  
    
    // ����irradiance map����diffuse��������
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse_ibl = irradiance * albedo;

    // ���ݴֲڶ���Ԥ���㻷����ͼ�Ͻ��в���
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;
    
    // ���ݴֲڶȺͷ��䷽���ȡbrdfֵ
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular_ibl = prefilteredColor * (F * brdf.x + brdf.y);

    // �õ����ջ�������
    vec3 ambient = (kD * diffuse_ibl + specular_ibl) * ao;
    

    // ---------------------------------------------------------------------
    
    // ���ս��=������+ֱ�ӹ�
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color , 1.0);
    //FragColor = vec4(specular_ibl, 1.0f);
}
