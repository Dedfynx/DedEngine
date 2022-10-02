#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>


namespace DedOs {
    class Window {
    public:
        Window(int w, int h, std::string n);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool shouldClose() {
            return glfwWindowShouldClose(window);
        }

        bool wasWindowResize() {
            return frameBufferResized;
        }

        void resetWindowResizeFlag() {
            frameBufferResized = false;
        }

        VkExtent2D getExtent() {
            return {
                static_cast<uint32_t>(width), 
                static_cast<uint32_t>(height) 
            };
        }

        GLFWwindow* getWindow() const { return window; }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    private:
        static void frameBufferResizeCallback(GLFWwindow *window, int width, int height);
        void initWindow();

        int width, height;
        bool frameBufferResized = false;
        std::string name;
        GLFWwindow* window;
    };

}
