#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// Vertex shader
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec3 offset;

void main()
{
    gl_Position = vec4(aPos + offset, 1.0);
    TexCoord = aTexCoord;
}
)";

// Fragment shader
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
)";

int main()
{
    // ----------------------------
    // 1️⃣ Initialize GLFW
    // ----------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Textured Triangles", NULL, NULL);
    if (!window) { std::cout << "Failed\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    // ----------------------------
    // 2️⃣ Initialize GLAD
    // ----------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cout << "GLAD failed\n"; return -1; }

    // ----------------------------
    // 3️⃣ Vertex data + texture coords
    // ----------------------------
    float vertices[] = {
        // positions         // tex coords
         0.0f,  0.5f, 0.0f,  0.5f, 1.0f, // first triangle top
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,

        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, // second triangle top
        -1.0f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.0f, -0.5f, 0.0f,  1.0f, 0.0f
    };

    // ----------------------------
    // 4️⃣ Setup VAO and VBO
    // ----------------------------
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ----------------------------
    // 5️⃣ Load texture
    // ----------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set texture wrapping/filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("image.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture\n";
    }
    stbi_image_free(data);

    // ----------------------------
    // 6️⃣ Compile shaders
    // ----------------------------
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //name-depends on fragmentshader/vertex shaders
    int offsetLoc = glGetUniformLocation(shaderProgram, "offset");
    int textureLoc = glGetUniformLocation(shaderProgram, "ourTexture");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ----------------------------
    // 7️⃣ Render loop
    // ----------------------------
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureLoc, 0);

        float time = (float)glfwGetTime();

        // draw first triangle (moving)
        glUniform3f(offsetLoc, 0.5f + sin(time) * 0.5f, 0.0f, 0.0f);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // draw second triangle (static)
        glUniform3f(offsetLoc, -0.5f, 0.0f, 0.0f);
        glDrawArrays(GL_TRIANGLES, 3, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ----------------------------
    // 8️⃣ Cleanup
    // ----------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &texture);

    glfwTerminate();
    return 0;
}

