#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>

class Input {
public:

	Input() = default;
	void processInput(GLFWwindow* window);
};

#endif