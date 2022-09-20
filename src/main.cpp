#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

#include "Shader.h"
#include "Camera.h"

using std::cout;
using std::endl;



// ���ڴ�С
constexpr GLuint WIDTH = 800, HEIGHT = 600;

// �����
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


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
    // ʵ����glfw����
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // ����opengl�汾
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // ����opengl�汾
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // ʹ�ú���ģʽ
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // ����glfw���ڶ���
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
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


    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    
    // ��Դλ��
    constexpr glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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
       // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

    
    // ��������1
    GLuint texture1;
    {
        // ��������
        glGenTextures(1, &texture1);

        // ��
        glBindTexture(GL_TEXTURE_2D, texture1);

        // �����ظ���ʽ
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // ���ò�ֵ��ʽ
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // ��ͼƬ���뵽����
        int tex_width, tex_height;
        unsigned char* image = SOIL_load_image("obj/container.jpg", &tex_width, &tex_height, 0, SOIL_LOAD_RGB);
        cout << tex_width << ' ' << tex_height << endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D); // ����mipmap

        // �ͷ��ڴ沢���
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // ��������2
    GLuint texture2;
    {
        // ��������
        glGenTextures(1, &texture2);

        // ��
        glBindTexture(GL_TEXTURE_2D, texture2);

        // �����ظ���ʽ
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // ���ò�ֵ��ʽ
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // ��ͼƬ���뵽����
        int tex_width, tex_height;
        unsigned char* image = SOIL_load_image("obj/awesomeface.jpg", &tex_width, &tex_height, 0, SOIL_LOAD_RGB);
        cout << tex_width << ' ' << tex_height << endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D); // ����mipmap 194802026228

        // �ͷ��ڴ沢���
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    
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

        // ���������״̬��ȡV��P����
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection;
        projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);

        // ��VAO
        glBindVertexArray(VAO);
        {
            /*
                ����box
            */

            // ����M����
            glm::mat4 model(1);
            model = glm::translate(model, glm::vec3(0, 0, 0));
            model = glm::rotate(model, (GLfloat)glfwGetTime() * 0.91f, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::rotate(model, (GLfloat)glfwGetTime() * 0.69f, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, (GLfloat)glfwGetTime() * 0.43f, glm::vec3(1.0f, 0.0f, 0.0f));

            // ������ɫ��
            shader_box.Use();

            // ��ȡ�任�����ַ
            GLint modelLoc = glGetUniformLocation(shader_box.Program, "model");
            GLint viewLoc = glGetUniformLocation(shader_box.Program, "view");
            GLint projLoc = glGetUniformLocation(shader_box.Program, "projection");

            // д��MVP����
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

            // ����box��light����ɫ
            GLint objectColorLoc = glGetUniformLocation(shader_box.Program, "objectColor");
            GLint lightColorLoc = glGetUniformLocation(shader_box.Program, "lightColor");
            glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);// ��������Ϥ��ɺ����
            glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // ���ɰѹ�Դ����Ϊ��ɫ

            // ����
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        {
            /*
                ����light
            */

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

