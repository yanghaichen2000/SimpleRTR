#version 330 core
out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;

in vec3 Normal;

void main()
{
    color = vec4(lightColor * objectColor, 1.0f);
}