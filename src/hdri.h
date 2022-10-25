#pragma once
#include "object.h"

class hdri {
public:

    // 以下为立方体贴图
    GLuint envCubemap; // 环境纹理
    GLuint irradianceMap; // 辐照度图
    GLuint prefilterMap; // 多粗糙度辐照度图

    // 以下为2D贴图
    GLuint brdfLUTTexture; // brdf查找表


	hdri(const char filename[])
	{
        // 预计算ibl贴图
        do_precompute(filename);
	}


private:
    void do_precompute(const char filename[]) {

        // 根据文件名读取环境纹理
        shared_ptr<texture_2D> tex_hdr = make_shared<texture_2D>(filename, texture_type::hdr);

        // 将环境纹理添加到材质
        shared_ptr<material> mat_hdr = make_shared<material>();
        mat_hdr->add_color_map(tex_hdr);

        // 读取立方体模型，用于将环境纹理转换为立方体贴图
        unordered_map<string, shared_ptr<material>> material_dict;
        material_dict[string("hdr_cube")] = mat_hdr;
        object object_hdr("obj/default/cube.obj", material_dict);
        object_hdr.buffer_data();
        
        // ---------------------------------------------------------------
        // 将hdri转换为立方体贴图

        // 启用立方体贴图之间的滤波
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        // 设置0级贴图大小
        int cube_map_size_0 = 1024;

        // 帧缓冲对象，用于记录圆柱投影转换到立方体贴图的结果
        unsigned int captureFBO, captureRBO;
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cube_map_size_0, cube_map_size_0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

        // 为立方体贴图分配缓存
        glGenTextures(1, &envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        for (unsigned int i = 0; i < 6; ++i)
        {
            // 格式为16位浮点数
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                cube_map_size_0, cube_map_size_0, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 设置V矩阵和P矩阵
        // P矩阵的fov为pi/2
        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        // V矩阵的ViewPos为原点，方向为六个坐标轴方向
        glm::mat4 captureViews[] =
        {
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        // 使用渲染hdri的shader
        Shader shader_hdr("src/shaders/vertex_hdr.glsl", "src/shaders/fragment_hdr.glsl");
        shader_hdr.Use();
        shader_hdr.set_uniform_mat4("projection", captureProjection);

        // 开始渲染六个面，并将渲染结果写入立方体贴图
        glViewport(0, 0, cube_map_size_0, cube_map_size_0);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (unsigned int i = 0; i < 6; ++i)
        {
            // 传入当前面的V矩阵
            shader_hdr.set_uniform_mat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDepthFunc(GL_LEQUAL);
            object_hdr.draw(shader_hdr);
            glDepthFunc(GL_LESS);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 为立方体贴图生成mipmap并设置三线性过滤，方便之后进行预滤波时使用
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);


        // ---------------------------------------------------------------
        // 根据立方体贴图计算辐照度图

        // 为辐照度图分配内存
        glGenTextures(1, &irradianceMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 帧缓冲对象，用于记录计算辐照度
        unsigned int irradianceFBO, irradianceRBO;
        glGenFramebuffers(1, &irradianceFBO);
        glGenRenderbuffers(1, &irradianceRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, irradianceRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, irradianceRBO);

        // 使用计算irradiance的shader
        Shader shader_irradiance("src/shaders/vertex_irradiance.glsl", "src/shaders/fragment_irradiance.glsl");
        shader_irradiance.Use();
        shader_irradiance.set_uniform_mat4("projection", captureProjection);

        // 传入之前计算的立方体贴图
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        // 开始渲染六个面，并将渲染结果写入立方体贴图
        glViewport(0, 0, 32, 32);
        glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);
        for (unsigned int i = 0; i < 6; ++i)
        {
            shader_irradiance.set_uniform_mat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDepthFunc(GL_LEQUAL);
            object_hdr.draw(shader_irradiance);
            glDepthFunc(GL_LESS);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // ---------------------------------------------------------------
        // 根据立方体贴图计算多粗糙度辐照度图

        // 分配内存

        glGenTextures(1, &prefilterMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, cube_map_size_0, cube_map_size_0, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP); // 声明mipmap

        // 帧缓冲对象，用于计算prefilter
        unsigned int prefilterFBO, prefilterRBO;
        glGenFramebuffers(1, &prefilterFBO);
        glGenRenderbuffers(1, &prefilterRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, prefilterFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, prefilterRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 128, 128); // 这里分辨率设成多少都行，之后会再更改
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, prefilterRBO);

        // 激活并设置shader
        Shader shader_prefilter_hdri("src/shaders/vertex_prefilter_hdri.glsl", "src/shaders/fragment_prefilter_hdri.glsl");
        shader_prefilter_hdri.Use();
        shader_prefilter_hdri.set_uniform_mat4("projection", captureProjection);

        // 传入之前计算的立方体贴图
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        // 开始渲染六个面
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        unsigned int maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
        {
            // 根据mipmap等级，设置渲染结果分辨率
            unsigned int mipWidth = cube_map_size_0 * std::pow(0.5, mip);
            unsigned int mipHeight = cube_map_size_0 * std::pow(0.5, mip);
            glBindRenderbuffer(GL_RENDERBUFFER, prefilterRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            shader_prefilter_hdri.set_uniform_float("roughness", roughness);
            for (unsigned int i = 0; i < 6; ++i)
            {
                shader_prefilter_hdri.set_uniform_mat4("view", captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glDepthFunc(GL_LEQUAL);
                object_hdr.draw(shader_prefilter_hdri);
                glDepthFunc(GL_LESS);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // ---------------------------------------------------------------
        // 预计算brdf

        // 为图像分配空间
        glGenTextures(1, &brdfLUTTexture);

        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 创建帧缓冲对象
        unsigned int brdfFBO, brdfRBO;
        glGenFramebuffers(1, &brdfFBO);
        glGenRenderbuffers(1, &brdfRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, brdfFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, brdfRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

        // 计算
        glViewport(0, 0, 512, 512);
        Shader shader_brdf("src/shaders/vertex_precompute_brdf.glsl", "src/shaders/fragment_precompute_brdf.glsl");
        shader_brdf.Use();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        unsigned int quadVAO, quadVBO;
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};