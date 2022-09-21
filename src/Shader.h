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
        // ��ȡvertex shader�ļ������Դ��
        string vertex_shader_path = vertexPath;
        string vertex_shader_string = readFileIntoString2(vertex_shader_path);
        const GLchar* vertex_shader_string_ptr = vertex_shader_string.c_str();

        // ����vertex shader
        GLuint vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);

        // װ��vertex shaderԴ�벢����
        glShaderSource(vertexShader, 1, &vertex_shader_string_ptr, nullptr);
        glCompileShader(vertexShader);

        // ���vertex shader�Ƿ����ɹ�
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

        // ��ȡfragment shader�ļ������Դ��
        string fragment_shader_path = fragmentPath;
        string fragment_shader_string = readFileIntoString2(fragment_shader_path);
        const GLchar* fragment_shader_string_ptr = fragment_shader_string.c_str();

        // ����fragment shader
        GLuint fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // װ��fragment shaderԴ�벢����
        glShaderSource(fragmentShader, 1, &fragment_shader_string_ptr, nullptr);
        glCompileShader(fragmentShader);

        // ���fragment shader�Ƿ����ɹ�
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            cout << "fragment shader compilation failed\n" << infoLog << endl;
        }
        else {
            cout << "fragment shader compilation succeeded\n";
        }

        // ������ɫ������
        Program = glCreateProgram();

        // ��vertex shader��fragment shader���ӵ���ɫ������
        glAttachShader(Program, vertexShader);
        glAttachShader(Program, fragmentShader);
        glLinkProgram(Program);

        // ��������Ƿ�ɹ�
        glGetProgramiv(Program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(Program, 512, NULL, infoLog);
            cout << "glLinkProgram failed\n" << infoLog << endl;
        }
        else {
            cout << "glLinkProgram succeeded\n";
        }

        // ɾ��shader����
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
	}
	
    void Use() {
        glUseProgram(Program);
    }
};
