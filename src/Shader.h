#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

using std::string;
using std::ifstream;
using std::ostringstream;
using std::cout;
using std::endl;


string readFileIntoString2(const string& path) {
    auto ss = ostringstream{};
    ifstream input_file(path);
    if (!input_file.is_open()) {
        cout << "Could not open the file - '" << path << "'\n";
        exit(EXIT_FAILURE);
    }
    ss << input_file.rdbuf();
    return ss.str();
}


class Shader {
public:
	GLuint Program;

public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
        // 读取vertex shader文件，获得源码
        string vertex_shader_path = vertexPath;
        string vertex_shader_string = readFileIntoString2(vertex_shader_path);
        const GLchar* vertex_shader_string_ptr = vertex_shader_string.c_str();

        // 创建vertex shader
        GLuint vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);

        // 装载vertex shader源码并编译
        glShaderSource(vertexShader, 1, &vertex_shader_string_ptr, nullptr);
        glCompileShader(vertexShader);

        // 检查vertex shader是否编译成功
        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            cout << "vertex shader compilation failed\n" << infoLog << endl;
        }
        else {
            cout << "vertex shader compilation succeeded\n";
        }

        // 读取fragment shader文件，获得源码
        string fragment_shader_path = fragmentPath;
        string fragment_shader_string = readFileIntoString2(fragment_shader_path);
        const GLchar* fragment_shader_string_ptr = fragment_shader_string.c_str();

        // 创建fragment shader
        GLuint fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // 装载fragment shader源码并编译
        glShaderSource(fragmentShader, 1, &fragment_shader_string_ptr, nullptr);
        glCompileShader(fragmentShader);

        // 检查fragment shader是否编译成功
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            cout << "fragment shader compilation failed\n" << infoLog << endl;
        }
        else {
            cout << "fragment shader compilation succeeded\n";
        }

        // 创建着色器程序
        Program = glCreateProgram();

        // 将vertex shader和fragment shader链接到着色器程序
        glAttachShader(Program, vertexShader);
        glAttachShader(Program, fragmentShader);
        glLinkProgram(Program);

        // 检查链接是否成功
        glGetProgramiv(Program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(Program, 512, NULL, infoLog);
            cout << "glLinkProgram failed\n" << infoLog << endl;
        }
        else {
            cout << "glLinkProgram succeeded\n";
        }

        // 删除shader对象
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
	}
	
    void Use() {
        glUseProgram(Program);
    }
};
