#ifndef INPUT_H
#define INPUT_H

#include "../includes/glad/glad.h"
#include "../includes/GLFW/glfw3.h"
#include "../headers/camera.h"

#include <iostream>
#include <string>

enum Valid_Input {
    ESC, W, S, A, D, SPACE, LEFT_SHIFT,
    UP_ARROW, DOWN_ARROW, F11,
    L, M, C, R, O, B,
    INPUT_COUNT   // always last, sizes the wasPressed array
};

class Input {
public:
    // toggles that main.cpp reads
    bool lightingEnabled = true;
    bool lightMovementEnabled = true;
    bool cellShadingEnabled = true;
    bool rimLightingEnabled = true;
    bool outlineEnabled = true;
    bool toggleBackground = true;
    float mixValue = 0.3f;

    Input(Camera& camera, float& deltaTime, float inputCooldownTime = 0.05f);

    void processInput(GLFWwindow* window);

private:
    Camera& camera;        // references: operate on main's real objects
    float& deltaTime;
    float inputCooldownTime;
    float mixCooldownTimer = 0.0f;

    bool isFullscreen = false;
    int windowedX = 0, windowedY = 0, windowedWidth = 0, windowedHeight = 0;

    bool wasPressed[INPUT_COUNT] = { false };

    void printInput(Valid_Input input);
    void toggleFullscreen(GLFWwindow* window);
};

#endif