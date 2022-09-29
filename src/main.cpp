#include "global.h"
#include "object.h"


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
    // ʵ����glfw����
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // ����opengl�汾
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // ����opengl�汾
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // ʹ�ú���ģʽ
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // ����glfw���ڶ���
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // ��ʼ��glew
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // ����glfw���ڳߴ����ô��ڴ�С��λ��
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);

    // ע�ᰴ���ص�����
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // ����shader
    Shader shader_box("src/shaders/vertex_box.glsl", "src/shaders/fragment_box.glsl");
    Shader shader_light("src/shaders/vertex_light.glsl", "src/shaders/fragment_light.glsl");
    Shader shader_depth("src/shaders/vertex_depth.glsl", "src/shaders/fragment_depth.glsl");

    
    // ����VBO��Vertex Buffer Objects��
    // VBO�д洢�˶��������
    GLuint VBO;
    glGenBuffers(1, &VBO); // 1��ʾ����һ�����󣬶�������ƣ�ID����洢�ڱ���"VBO"��

    // ����EBO��Element Buffer Object��
    // EBO�д洢�˶�����VBO�е�����
    GLuint EBO;
    glGenBuffers(1, &EBO);
    

    // ����VAO��Vertex Array Objects��
    // VAO�д洢�˶�������ָ��
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    

    // ��VAO
    glBindVertexArray(VAO); 
    {
        
        // ��VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // ���������ݴ���BUFFER
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // ��EBO
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // ���������ݴ���BUFFER
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // ���ö�������ָ��
        // λ������
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // ��������
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }
    // ���VAO
    glBindVertexArray(0);


    //int num_vertex;
    //GLuint VAO_2;
    //glGenVertexArrays(1, &VAO_2);

    // �������
    shared_ptr<material> mat_cow = make_shared<material>();
    shared_ptr<texture> tex_cow = make_shared<texture>("obj/spot_texture.png");
    mat_cow->add_color_map(tex_cow);
    
    shared_ptr<material> mat_white = make_shared<material>();
    shared_ptr<texture> tex_white = make_shared<texture>("obj/white.png");
    mat_white->add_color_map(tex_white);

    shared_ptr<material> mat_green = make_shared<material>();
    shared_ptr<texture> tex_green = make_shared<texture>("obj/green.png");
    mat_green->add_color_map(tex_green);

    shared_ptr<material> mat_red = make_shared<material>();
    shared_ptr<texture> tex_red = make_shared<texture>("obj/red.png");
    mat_red->add_color_map(tex_red);

    // ���ʱ�
    unordered_map<string, shared_ptr<material>> material_dict;
    material_dict[string("cube")] = mat_white;
    material_dict[string("cow")] = mat_cow;
    material_dict[string("plane_001")] = mat_red;
    material_dict[string("plane_002")] = mat_white;
    material_dict[string("plane_003")] = mat_white;
    material_dict[string("plane_004")] = mat_green;
    material_dict[string("plane_005")] = mat_white;


    // buffer������Ϣ
    object object_tmp("obj/test_cow.obj", material_dict);
    object_tmp.buffer_data();


    // ����shadow map
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


    // ����FBO
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    // ��shadowmap�󶨵�FBO
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // ��ȡ���ͼ��������ӵ�����materia��

    // ��Դ��M����
    glm::mat4 model_light(1);

    // ��Դ��V����
    glm::mat4 view_light = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // ��Դ��P����
    GLfloat near_plane = -1.0f, far_plane = 15.0f;
    glm::mat4 projection_light = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

    // �������ͼ��ɫ��
    shader_depth.Use();

    // uniform mat4 model
    GLint modelLoc = glGetUniformLocation(shader_depth.Program, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_light));

    // uniform mat4 view
    GLint viewLoc = glGetUniformLocation(shader_depth.Program, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_light));

    // uniform mat4 projection
    GLint projLoc = glGetUniformLocation(shader_depth.Program, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_light));

    // ���ӿڴ�С����Ϊ���ͼ��С
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    // �������Ⱦ�����ͼ
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        object_tmp.draw(shader_depth);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    // ��shadow_map����material��
    shared_ptr<texture> shadow_map_ptr = make_shared<texture>(depthMap);
    mat_cow->add_shadow_map(shadow_map_ptr);
    mat_white->add_shadow_map(shadow_map_ptr);
    mat_green->add_shadow_map(shadow_map_ptr);
    mat_red->add_shadow_map(shadow_map_ptr);
    
    
    glEnable(GL_DEPTH_TEST);
    // ��ѭ��
    while (!glfwWindowShouldClose(window)) {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        Do_Movement();

        // �����ɫ����
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // ���ӿڴ�С����Ϊ���ͼ��С
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

        // �������Ⱦ�����ͼ
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        {
            glClear(GL_DEPTH_BUFFER_BIT);
            object_tmp.draw(shader_depth);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // �����ɫ����
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       
        // ���������״̬��ȡV��P����
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection;
        projection = glm::perspective(camera.Zoom, (float)window_width / (float)window_height, 0.1f, 1000.0f);

        // M����
        glm::mat4 model(1);
        

        // ������ɫ��
        shader_box.Use();

        // ����uniform
        shader_box.set_uniform_mat4("model", model);
        shader_box.set_uniform_mat4("view", view);
        shader_box.set_uniform_mat4("projection", projection);

        shader_box.set_uniform_vec3("viewPos", camera.Position);
        shader_box.set_uniform_vec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader_box.set_uniform_vec3("lightPos", lightPos);
        shader_box.set_uniform_vec3("lightDir", 1.0f, 1.0f, 1.0f);

        shader_box.set_uniform_mat4("model_light", model_light);
        shader_box.set_uniform_mat4("view_light", view_light);
        shader_box.set_uniform_mat4("projection_light", projection_light);
        
        // ����box
        glViewport(0, 0, window_width, window_height);
        object_tmp.draw(shader_box);

        
        // ����light
        glBindVertexArray(VAO);
        {
            // ����M����
            glm::mat4 model(1);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.2));

            // ������ɫ��
            shader_light.Use();

            // ��ȡ�任�����ַ
            GLint modelLoc = glGetUniformLocation(shader_light.Program, "model");
            GLint viewLoc = glGetUniformLocation(shader_light.Program, "view");
            GLint projLoc = glGetUniformLocation(shader_light.Program, "projection");

            // д��MVP����
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            
            // ����
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // ���VAO
        glBindVertexArray(0);
      
        

        // ������ɫ����
        glfwSwapBuffers(window);
    }

    // �ͷ��ڴ�
    glfwTerminate();

    return 0;
}

