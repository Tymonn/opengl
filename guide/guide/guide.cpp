// guide.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <glad/glad.h>   //should be included before any use of opengl
#include "GLFW/glfw3.h"
#include <iostream>
#include <sstream>

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f,  0.5f, 0.0f
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, 600, 400);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(int argc, char** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Initialize the glad before any using of opengl funcs
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //copy the vertex data into the video card memory ,which now managed by VBO (vertex buffer object)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //now we need shaders to deal with the data
    //we create a vertex shader and a fragment shader, these two shaders are shaders which we must create in OpenGL

    //this is a vertex shader (actually a C-string) written by OpenGL Shading Language
    //vec3 is directly used to initialize gl_Position
    //But normally, the input coordinates are not NDC, we should transform them into NDC in the shader program
    std::stringstream shader_stream;
    shader_stream << "#version 330 core"<<std::endl;
    shader_stream << "layout (location = 0) in vec3 aPos;" << std::endl;
    shader_stream << "void main() {" << std::endl;
    shader_stream << "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);" << std::endl;
    shader_stream << "}" << std::endl;
    std::string string_source = shader_stream.str();
    const GLchar* source = string_source.c_str();
    
    //the shader will be referenced by an ID
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &source, NULL);
    glCompileShader(vertexShader);
    int  success = 0;
    char infoLog[512] = {0};
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    //success indicates whether the shader is compiled successfully
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return 1;
    }
    
    //fragment shader: the shader here always output orange color(color is expressed as RGBA in OpenGL)
    shader_stream.str("");
    shader_stream << "#version 330 core" << std::endl;
    shader_stream << "out vec4 FragColor;" << std::endl;
    shader_stream << "void main() {" << std::endl;
    shader_stream << "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);" << std::endl;
    shader_stream << "}" << std::endl;
    string_source = shader_stream.str();
    source = string_source.c_str();

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &source, NULL);
    glCompileShader(fragmentShader);
    success = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        memset(infoLog, 0, sizeof(infoLog));
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::Fragment::COMPILATION_FAILED\n" << infoLog << std::endl;

        return 1;
    }

    //Shader Program Object: the shaders must be linked into a shader program object
    //And the shader program object must be activated
    //the shaders of a activated shader program object will be executed when rendering
    unsigned int shaderProgram = glCreateProgram();
    //in sequence
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    //Link shaders into the program
    //it will fail if the output of a shader cannot match the input of the next shader
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        memset(infoLog, 0, sizeof(infoLog));
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //we should tell OpenGL how to link the vertex data into the vertex attributes of  the vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glViewport(0, 0, 600, 400);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();

    return 0;
}