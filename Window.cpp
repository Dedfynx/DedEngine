#include "Window.hpp"

#include <stdexcept>

namespace DedOs {
	Window::Window(int w, int h, std::string n) : width{ w }, height{ h }, name{ n }
	{
		initWindow();
	}

	Window::~Window()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Window Surface");
		}
	}

	void Window::frameBufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto tWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		tWindow->frameBufferResized = true;
		tWindow->width = width;
		tWindow->height = height;
	}

	void Window::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
	}

}
