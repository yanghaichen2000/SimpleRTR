#include "global.h"
#include "object.h"
#include "hdri.h"

//#define TEST_MODE_SIMPLERTR


#ifdef TEST_MODE_SIMPLERTR
int main() {
    stb::stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    // 遍历顺序：
    // 8 9 a b
    // 4 5 6 7
    // 0 1 2 3
    float* data = stb::stbi_loadf("obj/bridge.hdr", &width, &height, &nrComponents, 0);
    
    // 输出hdr图像
    //stb::stbi_write_hdr("obj/test_write.hdr", width, height, nrComponents, data);
}
#endif

#ifndef TEST_MODE_SIMPLERTR
void Do_Movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //cout << key << endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}



int main()
{
    // 实例化glfw窗口
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 设置opengl版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 设置opengl版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 使用核心模式
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // 创建glfw窗口对象
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化glew
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // 根据glfw窗口尺寸设置窗口大小和位置
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);

    // 注册按键回调函数
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 设置shader
    Shader shader_main("src/shaders/vertex_box.glsl", "src/shaders/fragment_box.glsl");
    Shader shader_light("src/shaders/vertex_light.glsl", "src/shaders/fragment_light.glsl");
    Shader shader_depth("src/shaders/vertex_depth.glsl", "src/shaders/fragment_depth.glsl");
    Shader shader_hdr("src/shaders/vertex_hdr.glsl", "src/shaders/fragment_hdr.glsl");
    Shader shader_hdrcube("src/shaders/vertex_hdrcube.glsl", "src/shaders/fragment_hdrcube.glsl");
    Shader shader_irradiance("src/shaders/vertex_irradiance.glsl", "src/shaders/fragment_irradiance.glsl");
    Shader shader_prefilter_hdri("src/shaders/vertex_prefilter_hdri.glsl", "src/shaders/fragment_prefilter_hdri.glsl");
    Shader shader_brdf("src/shaders/vertex_precompute_brdf.glsl", "src/shaders/fragment_precompute_brdf.glsl");
    Shader shader_pbr("src/shaders/vertex_pbr.glsl", "src/shaders/fragment_pbr.glsl");

    
    // 创建VBO（Vertex Buffer Objects）
    // VBO中存储了顶点的数据
    GLuint VBO;
    glGenBuffers(1, &VBO); // 1表示创建一个对象，对象的名称（ID）会存储在变量"VBO"中

    // 创建EBO（Element Buffer Object）
    // EBO中存储了顶点在VBO中的索引
    GLuint EBO;
    glGenBuffers(1, &EBO);
    

    // 创建VAO（Vertex Array Objects）
    // VAO中存储了顶点数据指针
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    

    // 绑定VAO
    glBindVertexArray(VAO); 
    {
        
        // 绑定VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // 将顶点数据传入BUFFER
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // 绑定EBO
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // 将索引数据传入BUFFER
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // 设置顶点数据指针
        // 位置属性
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // 纹理坐标
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }
    // 解绑VAO
    glBindVertexArray(0);


    //int num_vertex;
    //GLuint VAO_2;
    //glGenVertexArrays(1, &VAO_2);

    // 定义材质
    shared_ptr<material> mat_cow = make_shared<material>();
    shared_ptr<texture_2D> tex_cow = make_shared<texture_2D>("obj/spot_texture.png");
    mat_cow->add_color_map(tex_cow);
    
    shared_ptr<material> mat_white = make_shared<material>();
    shared_ptr<texture_2D> tex_white = make_shared<texture_2D>("obj/white.png");
    mat_white->add_color_map(tex_white);

    shared_ptr<material> mat_green = make_shared<material>();
    shared_ptr<texture_2D> tex_green = make_shared<texture_2D>("obj/green.png");
    mat_green->add_color_map(tex_green);

    shared_ptr<material> mat_red = make_shared<material>();
    shared_ptr<texture_2D> tex_red = make_shared<texture_2D>("obj/red.png");
    mat_red->add_color_map(tex_red);

    shared_ptr<material> mat_yellow = make_shared<material>();
    shared_ptr<texture_2D> tex_yellow = make_shared<texture_2D>("obj/yellow.png");
    mat_yellow->add_color_map(tex_yellow);

    shared_ptr<material> mat_hdr_test = make_shared<material>();
    shared_ptr<texture_2D> tex_hdr_test = make_shared<texture_2D>("obj/hdri/court.hdr", texture_type::hdr);
    mat_hdr_test->add_color_map(tex_hdr_test);

    shared_ptr<material> mat_pbr_test = make_shared<material>();
    mat_pbr_test->add_pbr_texture("obj/pbr/rustediron", "png");

    shared_ptr<material> mat_empty = make_shared<material>();

    // 材质表
    unordered_map<string, shared_ptr<material>> material_dict;
    material_dict[string("hdr_cube")] = mat_hdr_test;
    material_dict[string("cube")] = mat_pbr_test;
    material_dict[string("cow")] = mat_pbr_test;
    material_dict[string("plane_001")] = mat_red;
    material_dict[string("plane_002")] = mat_pbr_test;
    material_dict[string("plane_003")] = mat_pbr_test;
    material_dict[string("plane_004")] = mat_green;
    material_dict[string("plane_005")] = mat_pbr_test;


    // 读取并buffer模型
    object object_main("obj/test_cow_open.obj", material_dict);
    object_main.buffer_data();

    // 读取并buffer hdr模型
    object object_hdr("obj/test_cube.obj", material_dict);
    object_hdr.buffer_data();


    // 定义shadow map
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    // 创建FBO
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    // 将shadowmap绑定到FBO
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    

    // 获取深度图并将其添加到材质material中

    // 光源的M矩阵
    glm::mat4 model_light(1);

    // 光源的V矩阵
    glm::mat4 view_light = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // 光源的P矩阵
    GLfloat near_plane = -1.0f, far_plane = 15.0f;
    glm::mat4 projection_light = glm::ortho(-8.0f, 8.0f, -8.0f, 8.0f, near_plane, far_plane);

    // 激活深度图着色器
    shader_depth.Use();

    // 传入光源shader参数
    shader_depth.set_uniform_mat4("model", model_light);
    shader_depth.set_uniform_mat4("view", view_light);
    shader_depth.set_uniform_mat4("projection", projection_light);

    // 将视口大小设置为深度图大小
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    glEnable(GL_DEPTH_TEST);
    // 将深度渲染到深度图
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        object_main.draw(shader_depth);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    
    // 将shadow_map存入material中
    shared_ptr<texture_2D> shadow_map_ptr = make_shared<texture_2D>(depthMap);
    mat_cow->add_shadow_map(shadow_map_ptr);
    mat_white->add_shadow_map(shadow_map_ptr);
    mat_green->add_shadow_map(shadow_map_ptr);
    mat_red->add_shadow_map(shadow_map_ptr);
    mat_yellow->add_shadow_map(shadow_map_ptr);
    mat_pbr_test->add_shadow_map(shadow_map_ptr);
    

    // 读取环境纹理 
    hdri hdri_1("obj/hdri/court.hdr");
    
    // 为pbr shader设置环境光贴图
    mat_pbr_test->add_ibl_texture(hdri_1.irradianceMap, hdri_1.prefilterMap, hdri_1.brdfLUTTexture);
    

    // 为hdrcube shader设置贴图
    shader_hdrcube.Use();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, hdri_1.envCubemap);
    glUniform1i(glGetUniformLocation(shader_hdrcube.Program, "skybox"), 1);
    glBindTexture(GL_TEXTURE_2D, 0);


    // 主循环
    while (!glfwWindowShouldClose(window)) {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        Do_Movement();

        // 清空颜色缓冲
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // 将视口大小设置为深度图大小
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);


        // 清空颜色缓冲
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       
        // 根据摄像机状态获取V和P矩阵
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection;
        projection = glm::perspective(camera.Zoom, (float)window_width / (float)window_height, 0.1f, 1000.0f);

        // M矩阵
        glm::mat4 model(1);
        
        
        /*
        // 绘制场景(blinn phong)
        {
            // 激活着色器
            shader_main.Use();

            // 设置环境纹理
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
            glUniform1i(glGetUniformLocation(shader_main.Program, "environmentMap"), 2);

            // 设置uniform
            shader_main.set_uniform_mat4("model", model);
            shader_main.set_uniform_mat4("view", view);
            shader_main.set_uniform_mat4("projection", projection);

            shader_main.set_uniform_vec3("viewPos", camera.Position);
            shader_main.set_uniform_vec3("lightColor", 5.0f, 5.0f, 5.0f);
            shader_main.set_uniform_vec3("lightPos", lightPos);
            shader_main.set_uniform_vec3("lightDir", 1.0f, 1.0f, 1.0f);
            
            shader_main.set_uniform_mat4("model_light", model_light);
            shader_main.set_uniform_mat4("view_light", view_light);
            shader_main.set_uniform_mat4("projection_light", projection_light);

            // 绘制
            glViewport(0, 0, window_width, window_height);
            object_main.draw(shader_main);
        }
        */

        
        // 绘制场景(pbr)
        {
            // 激活着色器
            shader_pbr.Use();

            // 设置uniform
            shader_pbr.set_uniform_mat4("model", model);
            shader_pbr.set_uniform_mat4("view", view);
            shader_pbr.set_uniform_mat4("projection", projection);

            shader_pbr.set_uniform_vec3("viewPos", camera.Position);
            shader_pbr.set_uniform_vec3("lightColor", 5.0f, 5.0f, 5.0f);
            shader_pbr.set_uniform_vec3("lightPos", lightPos);
            shader_pbr.set_uniform_vec3("lightDir", 1.0f, 1.0f, 1.0f);

            shader_pbr.set_uniform_mat4("model_light", model_light);
            shader_pbr.set_uniform_mat4("view_light", view_light);
            shader_pbr.set_uniform_mat4("projection_light", projection_light);

            shader_pbr.set_uniform_vec3("camPos", camera.Position);

            // 绘制
            glViewport(0, 0, window_width, window_height);
            object_main.draw(shader_pbr);
        }
        
        
        /*
        // 绘制天空盒（直接从.hdr图绘制）
        {
            // 激活着色器
            shader_hdr.Use();

            // 设置uniform
            shader_hdr.set_uniform_mat4("view", view);
            shader_hdr.set_uniform_mat4("projection", projection);
            //shader_hdr.set_uniform_mat4("view", captureViews[3]);
            //shader_hdr.set_uniform_mat4("projection", captureProjection);

            // 绘制
            glViewport(0, 0, window_width, window_height);
            glDepthFunc(GL_LEQUAL); // 由于在shader中让天空盒的深度为1，所以需要加这一句
            object_hdr.draw(shader_hdr);
            glDepthFunc(GL_LESS);
        }
        */

        ///*
        // 绘制天空盒（根据转换的立方体贴图绘制）
        {
            shader_hdrcube.Use();
            
            shader_hdrcube.set_uniform_mat4("view", view);
            shader_hdrcube.set_uniform_mat4("projection", projection);

            glViewport(0, 0, window_width, window_height);
            glDepthFunc(GL_LEQUAL); // 由于在shader中让天空盒的深度为1，所以需要加这一句
            object_hdr.draw(shader_hdrcube);
            glDepthFunc(GL_LESS);
        }
        //*/
        
        
        // 绘制light
        glBindVertexArray(VAO);
        {
            // 计算M矩阵
            glm::mat4 model(1);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.2));

            // 激活着色器
            shader_light.Use();

            // 获取变换矩阵地址
            GLint modelLoc = glGetUniformLocation(shader_light.Program, "model");
            GLint viewLoc = glGetUniformLocation(shader_light.Program, "view");
            GLint projLoc = glGetUniformLocation(shader_light.Program, "projection");

            // 写入MVP矩阵
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

            // 绘制
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // 解绑VAO
        glBindVertexArray(0);
      
        

        // 交换颜色缓冲
        glfwSwapBuffers(window);
    }

    // 释放内存
    glfwTerminate();

    return 0;
}

#endif
