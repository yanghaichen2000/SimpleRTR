#pragma once
#include "global.h"
#include "Shader.h"

class texture {
public:

	GLuint texture_id;
	
	texture(const char filename[], GLint repeat = GL_REPEAT, GLint interpolate = GL_LINEAR) {
        // 创建纹理
        glGenTextures(1, &texture_id);

        // 绑定
        glBindTexture(GL_TEXTURE_2D, texture_id);

        // 设置重复方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // 设置插值方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 将图片载入到纹理
        int tex_width, tex_height;
        unsigned char* image = SOIL_load_image(filename, &tex_width, &tex_height, 0, SOIL_LOAD_RGB);
        cout << "texture::init() texture loaded: " << filename << ", size = " <<  tex_width << 'x' << tex_height << endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D); // 生成mipmap

        // 释放内存并解绑
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
	}
    
    // 将该纹理设置为uniform变量
    void make_uniform(Shader shader, const char name_in_shader[], int index) {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glUniform1i(glGetUniformLocation(shader.Program, name_in_shader), index);
    }
};