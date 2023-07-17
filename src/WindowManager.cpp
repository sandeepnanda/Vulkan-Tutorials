#include "WindowManager.h"

WindowManager::WindowManager()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(800, 600, "VulkanTriangle", nullptr, nullptr);
}

WindowManager::WindowManager(uint32_t width, uint32_t height, std::string name)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); 
    m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
}

WindowManager::~WindowManager()
{
    if (m_window) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        m_window = nullptr;
    }
}

void WindowManager::KeepRunning()
{
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
    }
}
