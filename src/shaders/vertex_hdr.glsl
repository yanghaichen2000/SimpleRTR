#version 330 core
layout (location = 0) in vec3 position;

out vec3 localPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    localPos = position;  
    mat4 view_without_translation = mat4(mat3(view));
    gl_Position = (projection * view_without_translation * vec4(localPos, 1.0)).xyww; // .xyww是为了使深度恒为1
}