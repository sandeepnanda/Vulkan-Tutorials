#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

class WindowManager
{
public:
    WindowManager();
    WindowManager(uint32_t width, uint32_t height, std::string name);
    ~WindowManager();
    void KeepRunning();
    GLFWwindow* GetWindow( ) { return m_window; }

private:
    GLFWwindow* m_window = nullptr;
};

