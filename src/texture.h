#pragma once
#include "global.h"
#include "Shader.h"

struct texture_type{
    enum {rgb, hdr};
};

class texture {
public:

	GLuint texture_id;
	
    // ��ͼƬ������
	texture(const char filename[], int type = texture_type::rgb, GLint repeat = GL_REPEAT, GLint interpolate = GL_LINEAR) {
        
        if (type == texture_type::rgb) {
            // ��������
            glGenTextures(1, &texture_id);

            // ��
            glBindTexture(GL_TEXTURE_2D, texture_id);

            // �����ظ���ʽ
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // ���ò�ֵ��ʽ
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // ��ͼƬ���뵽����
            int tex_width, tex_height, nrComponents;
            unsigned char* image = stb::stbi_load(filename, &tex_width, &tex_height, &nrComponents, 3);
            if (image) {
                cout << "texture::init() texture loaded: " << filename << ", size = " << tex_width << 'x' << tex_height << endl;
            }
            else {
                cout << "texture::init() failed to load texture: " << filename << endl;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_2D); // ����mipmap

            // �ͷ��ڴ沢���
            stb::stbi_image_free(image);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else if (type == texture_type::hdr) {
            // ��������
            glGenTextures(1, &texture_id);

            // ��
            glBindTexture(GL_TEXTURE_2D, texture_id);

            // �����ظ���ʽ
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // ���ò�ֵ��ʽ
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // ��ͼƬ���뵽����
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

            // �ͷ��ڴ沢���
            stb::stbi_image_free(image);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else {
            cout << "texture::init() error: unknown texture type = " << type << endl;
            texture_id = 0;
        }
	}

    // ��������texture
    texture(GLuint texture_id_init) {
        texture_id = texture_id_init;

        cout << "texture::init() texture loaded from gl_texture";
    }
    
    // ������������Ϊuniform����
    void make_uniform(Shader shader, const char name_in_shader[], int index) {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glUniform1i(glGetUniformLocation(shader.Program, name_in_shader), index);
    }
};