#include "global.h"
#include "object.h"


// Moves/alters the camera positions based on user input
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

// Is called whenever a key is pressed/released via GLFW
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
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

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
    Shader shader_box("src/shaders/vertex_box.glsl", "src/shaders/fragment_box.glsl");
    Shader shader_light("src/shaders/vertex_light.glsl", "src/shaders/fragment_light.glsl");


    
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

    // 材质表
    unordered_map<string, shared_ptr<material>> material_dict;
    material_dict[string("cube")] = mat_white;
    material_dict[string("cow")] = mat_cow;
    material_dict[string("plane_001")] = mat_red;
    material_dict[string("plane_002")] = mat_white;
    material_dict[string("plane_003")] = mat_white;
    material_dict[string("plane_004")] = mat_green;
    material_dict[string("plane_005")] = mat_white;

    object object_tmp("obj/test_cow.obj", material_dict);
    object_tmp.buffer_data();

    
    glEnable(GL_DEPTH_TEST);
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

        // 根据摄像机状态获取V和P矩阵
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection;
        projection = glm::perspective(camera.Zoom, (float)window_width / (float)window_height, 0.1f, 1000.0f);

        // 绑定VAO
        // 计算M矩阵
        glm::mat4 model(1);
        //model = glm::translate(model, glm::vec3(0, 0, 0));
        //model = glm::rotate(model, (GLfloat)glfwGetTime() * 0.91f, glm::vec3(0.0f, 0.0f, 1.0f));
        //model = glm::rotate(model, (GLfloat)glfwGetTime() * 0.69f, glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::rotate(model, (GLfloat)glfwGetTime() * 0.43f, glm::vec3(1.0f, 0.0f, 0.0f));

        // 激活着色器
        shader_box.Use();

        // uniform mat4 model
        GLint modelLoc = glGetUniformLocation(shader_box.Program, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // uniform mat4 view
        GLint viewLoc = glGetUniformLocation(shader_box.Program, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // uniform mat4 projection
        GLint projLoc = glGetUniformLocation(shader_box.Program, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // uniform vec3 objectColor
        GLint objectColorLoc = glGetUniformLocation(shader_box.Program, "objectColor");
        glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

        // uniform vec3 viewPos
        GLint viewPosLoc = glGetUniformLocation(shader_box.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

        // uniform vec3 lightColor
        GLint lightColorLoc = glGetUniformLocation(shader_box.Program, "lightColor");
        glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

        // uniform vec3 lightPos
        GLint lightPosLoc = glGetUniformLocation(shader_box.Program, "lightPos");
        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);


        /*
            绘制box
        */
        object_tmp.draw(shader_box);

        /*
            绘制light
        */
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

