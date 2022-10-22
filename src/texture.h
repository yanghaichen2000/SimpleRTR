#pragma once
#include "global.h"
#include "Shader.h"

struct texture_type{
    enum {rgb, hdr};
};

class texture {
public:

	GLuint texture_id;
	
    // 从图片中载入
	texture(const char filename[], int type = texture_type::rgb, GLint repeat = GL_REPEAT, GLint interpolate = GL_LINEAR) {
        
        if (type == texture_type::rgb) {
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
            int tex_width, tex_height, nrComponents;
            unsigned char* image = stb::stbi_load(filename, &tex_width, &tex_height, &nrComponents, 3);
            if (image) {
                cout << "texture::init() texture loaded: " << filename << ", size = " << tex_width << 'x' << tex_height << endl;
            }
            else {
                cout << "texture::init() failed to load texture: " << filename << endl;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_2D); // 生成mipmap

            // 释放内存并解绑
            stb::stbi_image_free(image);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else if (type == texture_type::hdr) {
            // 创建纹理
            glGenTextures(1, &texture_id);

            // 绑定
            glBindTexture(GL_TEXTURE_2D, texture_id);

            // 设置重复方式
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // 设置插值方式
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // 将图片载入到纹理
            int tex_width, tex_height, nrComponents;
            stb::stbi_set_flip_vertically_on_load(true);
            float* image = stb::stbi_loadf(filename, &tex_width, &tex_height, &nrComponents, 0);
            if (image) {
                cout << "texture::init() HDR texture loaded: " << filename << ", size = " << tex_width << 'x' << tex_height << endl;
            }
            else {
                cout << "texture::init() failed to load HDR texture: " << filename << endl;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, tex_width, tex_height, 0, GL_RGB, GL_FLOAT, image);

            // 释放内存并解绑
            stb::stbi_image_free(image);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else {
            cout << "texture::init() error: unknown texture type = " << type << endl;
            texture_id = 0;
        }
	}

    // 设置现有texture
    texture(GLuint texture_id_init) {
        texture_id = texture_id_init;

        cout << "texture::init() texture loaded from gl_texture";
    }
    
    // 将该纹理设置为uniform变量
    void make_uniform(Shader shader, const char name_in_shader[], int index) {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glUniform1i(glGetUniformLocation(shader.Program, name_in_shader), index);
    }
};