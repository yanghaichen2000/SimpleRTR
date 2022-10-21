#version 330 core
out vec4 FragColor;
in vec3 localPos;

uniform sampler2D color_map_0;


const vec2 invAtan = vec2(0.1591, 0.3183);
// 根据方向计算环境纹理的uv
// v必须要归一化
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{       
    vec2 uv = SampleSphericalMap(normalize(localPos)); // make sure to normalize localPos
    vec3 color = texture(color_map_0, uv).rgb;
    //color = vec3(pow(color.x, 1.0f / 2.2f), pow(color.y, 1.0f / 2.2f), pow(color.z, 1.0f / 2.2f));
    FragColor = vec4(color, 1.0);
}
