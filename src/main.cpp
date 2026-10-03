#include "../includes/glad/glad.h"
#include "../includes/GLFW/glfw3.h"

#include "../includes/glm/glm.hpp"
#include "../includes/glm/gtc/matrix_transform.hpp"
#include "../includes/glm/gtc/type_ptr.hpp"

#include "../headers/camera.h"
#include "../headers/input.h"
#include "../headers/model.h"
#include "../headers/shader_s.h"

#include <cmath>
#include <iostream>
#include <string>

void updatePerformanceCounter(GLFWwindow* window);
void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(
    const char* path,
    GLenum wrapMode = GL_REPEAT,
    GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR,
    GLenum magFilter = GL_LINEAR
);

// screen size
unsigned int SCR_WIDTH = 853;
unsigned int SCR_HEIGHT = 480;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0f;
float lastY = (float)SCR_HEIGHT / 2.0f;
bool firstMouseInput = true;

// frame times
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

Input input(camera, deltaTime);

glm::vec3 lightPos(1.2f, 1.4f, 0.8f);
glm::vec3 lightDir(-0.7f, -0.2f, -1.8f);
glm::vec3 whiteLightColor(1.0f);                                // #FFFFFF
glm::vec3 sunLightColor(1.0f, 0.988f, 0.924f);                  // #FFFCEB
glm::vec3 ambientLightColor(0.706f, 0.843f, 1.0f);              // #B4D7FF

glm::vec3 blackOutlineColor(0.0f);                              // #000000
glm::vec3 whiteOutlineColor(1.0f);                              // #FFFFFF
glm::vec3 darkBrownOutlineColor(0.176f, 0.118f, 0.098f);        // #2D1E19
glm::vec3 darkRedOutlineColor(0.235f, 0.255f, 0.314f);          // #3C4150
glm::vec3 darkBlueOutlineColor(0.314f, 0.137f, 0.137f);         // #502323

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "main", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  // write the reference value when stencil and depth pass
    glfwSwapInterval(0);                        // disable vsync

    Shader lightingShader("resources/shaders/3.3.light.vs", "resources/shaders/3.3.light.fs");
    Shader lightCubeShader("resources/shaders/3.3.light_cube.vs", "resources/shaders/3.3.light_cube.fs");
    Shader outlineShader("resources/shaders/3.3.outline.vs", "resources/shaders/3.3.outline.fs");

    Model nijika("resources/objects/ijichi-nijika/1.fbx");

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    unsigned int VBO, modelVAO;
    glGenVertexArrays(1, &modelVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(modelVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    const float modelScale = 0.007f;
    const float outlineWidth = 0.004f;  // world units if your outline.vs extrudes in world space

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        input.processInput(window);

        // background
        if (input.toggleBackground)
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);    // dark
        else
            glClearColor(0.95f, 0.95f, 0.95f, 1.0f);    // light

        glStencilMask(0xFF);    // the stencil mask must allow writes or the clear is ignored
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // view/projection transformations
        float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), aspect, nearPlane, farPlane);
        glm::mat4 view = camera.GetViewMatrix();

        // pass 1: render the model and write 1s into the stencil buffer
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        lightingShader.setBool("gLightingEnabled", input.lightingEnabled);
        lightingShader.setBool("gCellShadingEnabled", input.cellShadingEnabled);
        lightingShader.setBool("gRimLightingEnabled", input.rimLightingEnabled);

        lightingShader.setVec3("material.specular", glm::vec3(0.2f));
        lightingShader.setFloat("material.shininess", 8.0f);

        // directional light
        if (input.lightMovementEnabled)
        {
            float speed = 0.6f;
            float amplitude = 0.8f;
            lightDir.x = std::sin(static_cast<float>(glfwGetTime()) * speed) * amplitude;
        }

        lightingShader.setVec3("dirLight.direction", lightDir);
        lightingShader.setVec3("dirLight.ambient", glm::vec3(0.3f));
        lightingShader.setVec3("dirLight.diffuse", glm::vec3(0.3f));
        lightingShader.setVec3("dirLight.specular", glm::vec3(0.5f));
        lightingShader.setVec3("dirLight.color", whiteLightColor);

        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.8f, 0.0f));
        model = glm::scale(model, glm::vec3(modelScale));
        lightingShader.setMat4("model", model);
        nijika.Draw(lightingShader);

        // pass 2: render the extruded model only where the stencil is not 1
        if (input.outlineEnabled)
        {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);    // don't modify the stencil buffer
            // depth test stays enabled so the outline hides behind other objects

            outlineShader.use();

            glm::vec3 currentOutlineColor = input.toggleBackground ? whiteOutlineColor : darkBrownOutlineColor;

            outlineShader.setVec3("outlineColor", currentOutlineColor);
            outlineShader.setFloat("outlineWidth", outlineWidth);
            outlineShader.setMat4("projection", projection);
            outlineShader.setMat4("view", view);
            outlineShader.setMat4("model", model);
            nijika.Draw(outlineShader);

            // reset state
            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        updatePerformanceCounter(window);
    }

    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteVertexArrays(1, &modelVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void updatePerformanceCounter(GLFWwindow* window)
{
    static double lastTime = glfwGetTime();
    static int frameCount = 0;

    double currentTime = glfwGetTime();
    frameCount++;

    if (currentTime - lastTime >= 1.0)
    {
        float fps = float(frameCount) / (currentTime - lastTime);
        float msPerFrame = (currentTime - lastTime) * 1000.0f / float(frameCount);

        std::string title = "main | fps: " + std::to_string(int(fps)) + " | time: " + std::to_string(msPerFrame) + " /ms";
        glfwSetWindowTitle(window, title.c_str());

        frameCount = 0;
        lastTime = currentTime;
    }
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
{
    float xpos = static_cast<float>(xPosIn);
    float ypos = static_cast<float>(yPosIn);

    if (firstMouseInput)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouseInput = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(const char* path, GLenum wrapMode, GLenum minFilter, GLenum magFilter)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}