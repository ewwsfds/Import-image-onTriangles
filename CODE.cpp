#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// Vertex shader
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;

uniform vec3 offset;

void main()
{
    gl_Position = vec4(aPos + offset, 1.0);
}
)";

// Fragment shader
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.2, 0.8, 0.3, 1.0); // greenish color
}
)";

void addQuad(std::vector<float>& vertices, float x, float y, float w, float h)
{
    // top-left, bottom-left, bottom-right
    vertices.push_back(x);     vertices.push_back(y + h); vertices.push_back(0.0f);
    vertices.push_back(x);     vertices.push_back(y);     vertices.push_back(0.0f);
    vertices.push_back(x + w); vertices.push_back(y);     vertices.push_back(0.0f);

    // top-left, bottom-right, top-right
    vertices.push_back(x);     vertices.push_back(y + h); vertices.push_back(0.0f);
    vertices.push_back(x + w); vertices.push_back(y);     vertices.push_back(0.0f);
    vertices.push_back(x + w); vertices.push_back(y + h); vertices.push_back(0.0f);
}

int main()
{
    // 1️⃣ Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangle", NULL, NULL);
    if (!window) { std::cout << "Failed\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    // 2️⃣ Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD failed\n"; return -1;
    }




    // 3️⃣ Vertex data
    std::vector<float> vertices;


    //Draw Quad function recall
    addQuad(vertices, -0.5f, -0.5f, 1.0f, 1.0f); // adds one quad

    // 4️⃣ Setup VAO and VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 5️⃣ Compile shaders
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

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int offsetLoc = glGetUniformLocation(shaderProgram, "offset");

    // 6️⃣ Render loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        float time = (float)glfwGetTime();
        glUniform3f(offsetLoc, sin(time) * 0.5f, 0.0f, 0.0f);

        glDrawArrays(GL_TRIANGLES, 0, 6); // first triangle


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 7️⃣ Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
