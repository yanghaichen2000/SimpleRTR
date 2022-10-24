#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float lod_value;

void main()
{    
    vec3 envColor = texture(skybox, TexCoords).rgb;

    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 

    //FragColor = vec4(envColor, 1.0);
    FragColor = vec4(envColor, 1.0f);
}
