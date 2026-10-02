#include "../headers/input.h"

Input::Input(Camera& camera, float& deltaTime, float inputCooldownTime)
    : camera(camera), deltaTime(deltaTime), inputCooldownTime(inputCooldownTime)
{
}

void Input::processInput(GLFWwindow* window)
{
    bool escDown = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    if (escDown)
    {
        if (!wasPressed[ESC]) printInput(ESC);
        glfwSetWindowShouldClose(window, true);
    }
    wasPressed[ESC] = escDown;

    // movement
    bool wDown = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    if (wDown)
    {
        if (!wasPressed[W]) printInput(W);
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    wasPressed[W] = wDown;

    bool sDown = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    if (sDown)
    {
        if (!wasPressed[S]) printInput(S);
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    wasPressed[S] = sDown;

    bool aDown = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    if (aDown)
    {
        if (!wasPressed[A]) printInput(A);
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    wasPressed[A] = aDown;

    bool dDown = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    if (dDown)
    {
        if (!wasPressed[D]) printInput(D);
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    wasPressed[D] = dDown;

    bool spaceDown = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (spaceDown)
    {
        if (!wasPressed[SPACE]) printInput(SPACE);
        camera.ProcessKeyboard(UP, deltaTime);
    }
    wasPressed[SPACE] = spaceDown;

    bool shiftDown = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    if (shiftDown)
    {
        if (!wasPressed[LEFT_SHIFT]) printInput(LEFT_SHIFT);
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    wasPressed[LEFT_SHIFT] = shiftDown;

    // texture mix
    mixCooldownTimer += deltaTime;
    bool upDown = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
    if (upDown && mixCooldownTimer >= inputCooldownTime)
    {
        if (!wasPressed[UP_ARROW]) printInput(UP_ARROW);
        mixValue += 0.05f;
        if (mixValue >= 1.0f) mixValue = 1.0f;
        mixCooldownTimer = 0.0f;
    }
    wasPressed[UP_ARROW] = upDown;

    bool downDown = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
    if (downDown && mixCooldownTimer >= inputCooldownTime)
    {
        if (!wasPressed[DOWN_ARROW]) printInput(DOWN_ARROW);
        mixValue -= 0.05f;
        if (mixValue <= 0.0f) mixValue = 0.0f;
        mixCooldownTimer = 0.0f;
    }
    wasPressed[DOWN_ARROW] = downDown;

    // fullscreen
    bool f11Down = glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS;
    if (f11Down && !wasPressed[F11]) {
        printInput(F11);
        toggleFullscreen(window);
    }
    wasPressed[F11] = f11Down;

    // toggle background
    bool bDown = glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS;
    if (bDown && !wasPressed[B]) {
        printInput(B);
        toggleBackground = !toggleBackground;
    }
    wasPressed[B] = bDown;

    // toggle lighting
    bool lDown = glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS;
    if (lDown && !wasPressed[L]) {
        printInput(L);
        lightingEnabled = !lightingEnabled;
    }
    wasPressed[L] = lDown;

    // move light
    bool mDown = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;
    if (mDown && !wasPressed[M]) {
        printInput(M);
        lightMovementEnabled = !lightMovementEnabled;
    }
    wasPressed[M] = mDown;

    // toggle cel shading
    bool cDown = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;
    if (cDown && !wasPressed[C]) {
        printInput(C);
        cellShadingEnabled = !cellShadingEnabled;
    }
    wasPressed[C] = cDown;

    // toggle rim lighting
    bool rDown = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
    if (rDown && !wasPressed[R]) {
        printInput(R);
        rimLightingEnabled = !rimLightingEnabled;
    }
    wasPressed[R] = rDown;

    // toggle outline
    bool oDown = glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS;
    if (oDown && !wasPressed[O]) {
        printInput(O);
        outlineEnabled = !outlineEnabled;
    }
    wasPressed[O] = oDown;
}

void Input::printInput(Valid_Input input)
{
    std::string currentInput = "";

    if (input == ESC)
        currentInput = "ESC";
    if (input == W) 
        currentInput = "W";
    if (input == S) 
        currentInput = "S";
    if (input == A) 
        currentInput = "A";
    if (input == D) 
        currentInput = "D";
    if (input == SPACE) 
        currentInput = "SPACE";
    if (input == LEFT_SHIFT) 
        currentInput = "LEFT_SHIFT";
    if (input == UP_ARROW) 
        currentInput = "ARROW_UP";
    if (input == DOWN_ARROW) 
        currentInput = "ARROW_DOWN";
    if (input == F11) 
        currentInput = "F11";
    if (input == B) 
        currentInput = "B";
    if (input == L) 
        currentInput = "L";
    if (input == M) 
        currentInput = "M";
    if (input == C)
        currentInput = "C";
    if (input == R)
        currentInput = "R";
    if (input == O)
        currentInput = "O";

    std::cout << "\rinput: " << currentInput << "          " << std::flush;
}

void Input::toggleFullscreen(GLFWwindow* window)
{
    if (!isFullscreen)
    {
        glfwGetWindowPos(window, &windowedX, &windowedY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else
        glfwSetWindowMonitor(window, NULL, windowedX, windowedY, windowedWidth, windowedHeight, 0);
    isFullscreen = !isFullscreen;
}