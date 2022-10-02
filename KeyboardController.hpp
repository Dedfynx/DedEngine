#pragma once

#include "GameObject.hpp"
#include "Window.hpp"

namespace DedOs {
    class KeyboardController {
    public:
        struct KeyMap {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_SPACE;
            int moveDown = GLFW_KEY_LEFT_SHIFT;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        KeyMap keys{};
        float moveSpeed{ 3.f };
        float lookSpeed{ 1.5f };

        void moveXZ(GLFWwindow* window, float dt, GameObject& gameObject);
	private:
	};
}