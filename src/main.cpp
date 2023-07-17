#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLM/glm.hpp>
#include <GLM/mat4x4.hpp>

#include <iostream>
#include <stdexcept>

#include "WindowManager.h"
#include "VulkanRenderer.h"

int main()
{
    try {
        WindowManager win;
        VulkanApp::VulkanRenderer app( VulkanApp::DISCRETE , win.GetWindow());
        win.KeepRunning();
        return 0;
    }
    catch (const std::runtime_error& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}