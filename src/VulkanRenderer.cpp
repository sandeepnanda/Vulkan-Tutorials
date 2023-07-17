#include "VulkanRenderer.h"



namespace VulkanApp
{
    const bool enableValidationLayer = true;
    std::vector< const char*> instanceLayerRequired = {
        "VK_LAYER_KHRONOS_validation",
        //"VK_LAYER_LUNARG_api_dump"
    };

    std::vector<const char*> deviceExtensionRequired = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VulkanRenderer::VulkanRenderer( GPU_TYPE gpuType, GLFWwindow* window ):
        m_window(window )
    {
        CreateVulkanInstance( );
        GetVukanPhysicalDevice( gpuType );
        CreateVulkanLogicalDevice( );
        CreateSurface( );
        CreateSwapChain( );
    }

    VulkanRenderer::~VulkanRenderer( )
    {
        for (auto imageView : m_swapchain.imageViewList) {
            vkDestroyImageView( m_device.logicalDevice, imageView, nullptr );
        }
        vkDestroySwapchainKHR( m_device.logicalDevice, m_swapchain.handle, nullptr );
        vkDestroySurfaceKHR( m_instance, m_surface, nullptr );
        vkDeviceWaitIdle( m_device.logicalDevice );
        vkDestroyDevice( m_device.logicalDevice, nullptr );
        vkDestroyInstance( m_instance, nullptr );
    }

    void VulkanRenderer::CreateVulkanInstance( )
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Triangle App";
        appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
        appInfo.apiVersion = VK_API_VERSION_1_3;


        uint32_t extCount = 0;
        const char** extNames = nullptr;
        extNames = glfwGetRequiredInstanceExtensions( &extCount );

        VkInstanceCreateInfo insInfo = {};
        insInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        insInfo.pApplicationInfo = &appInfo;
        insInfo.ppEnabledExtensionNames = extNames;
        insInfo.enabledExtensionCount = extCount;
        if (enableValidationLayer && IsSupportedInstanceLayer()) {
           

            insInfo.enabledLayerCount = static_cast<uint32_t> ( instanceLayerRequired.size());
            insInfo.ppEnabledLayerNames = instanceLayerRequired.data( );
        } else {
            insInfo.enabledLayerCount = 0;
            insInfo.ppEnabledLayerNames = nullptr;
        }

        if (vkCreateInstance( &insInfo, nullptr, &m_instance ) != VK_SUCCESS)
            throw std::runtime_error( "VR Error : Instance creation Failed!" );
    }

    void VulkanRenderer::GetVukanPhysicalDevice( GPU_TYPE& gpuType )
    {
        uint32_t phyDeviceCount = 0;

        vkEnumeratePhysicalDevices( m_instance, &phyDeviceCount, nullptr );

        std::vector<VkPhysicalDevice> phyDevices( phyDeviceCount );

        vkEnumeratePhysicalDevices( m_instance, &phyDeviceCount, phyDevices.data( ) );
        

        std::vector<VkPhysicalDeviceProperties> deviceProperties( phyDeviceCount );

        int32_t supportedDeviceIndex = -1;
        for (uint32_t i = 0; i < phyDeviceCount; i++) {
            vkGetPhysicalDeviceProperties( phyDevices[i], &deviceProperties[i] );

            if (gpuType == INTEGRATED) {
                if (deviceProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
                    supportedDeviceIndex = i;
            } else if (gpuType == DISCRETE) {
                if (deviceProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                    supportedDeviceIndex = i;
                }
            }
        }

        if (supportedDeviceIndex < 0) {
            throw std::runtime_error( "VR Error: Requested Physical Device not supported !" );
        }
        m_device.physicalDevice = phyDevices[supportedDeviceIndex];

        // Fill Queue indices 
        uint32_t queueFamilycount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( m_device.physicalDevice, &queueFamilycount, nullptr );
        std::vector<VkQueueFamilyProperties> queueFamilyList( queueFamilycount );

        vkGetPhysicalDeviceQueueFamilyProperties( m_device.physicalDevice, &queueFamilycount, queueFamilyList.data( ) );

        int i = 0;
        for (const auto &queueFamily : queueFamilyList) {
            if ((queueFamily.queueCount > 0) && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
                m_queueFamily.indices[GRAPHICS] = i;
            else if ((queueFamily.queueCount > 0) && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))
                m_queueFamily.indices[COMPUTE] = i;
            else if ((queueFamily.queueCount > 0) && (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT))
                m_queueFamily.indices[TRANSFER] = i;

            i++;
        }
    }

    bool VulkanRenderer::IsSupportedInstanceLayer( )
    {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

        std::vector < VkLayerProperties> supportedLayersList( layerCount );
        vkEnumerateInstanceLayerProperties( &layerCount, supportedLayersList.data( ) );

        for (const auto& requiredLayer : instanceLayerRequired) {
            bool supported = false;

            for (const auto& layer : supportedLayersList) {
                if (strcmp(requiredLayer,layer.layerName) == 0 ) {
                    supported = true;
                    break;
                }
            }
            
            if (!supported) {
                return false;
            }
        }
        return true;
    }

    bool VulkanRenderer::IsSupportedDeviceExtension( VkPhysicalDevice& device)
    {
        uint32_t devExtensionCount = 0;
        vkEnumerateDeviceExtensionProperties( device, nullptr, &devExtensionCount, nullptr );

        std::vector<VkExtensionProperties>  supportedExtList( devExtensionCount );

        vkEnumerateDeviceExtensionProperties( device, nullptr, &devExtensionCount, supportedExtList.data() );

        for (auto& requitedExt : deviceExtensionRequired) {
            bool supported = false;
            for (auto& supportedExt : supportedExtList) {
                if (strcmp(requitedExt, supportedExt.extensionName) == 0) {
                    supported = true;
                    break;
                }
            }

            if (!supported) {
                return false;
            }
        }

        return true;
    }

    void VulkanRenderer::CreateVulkanLogicalDevice( )
    {
        // Populate queue info list
        std::vector<VkDeviceQueueCreateInfo> queueInfoList;

        for (uint32_t i = 0; i < MAX_QUEUE_FAMILY_COUNT; i++) {
            auto queueFamilyIndex = static_cast<QueueFamily> (i);
            if (m_queueFamily.isValid( queueFamilyIndex )) {
                VkDeviceQueueCreateInfo queueInfo = {};
                queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueInfo.queueFamilyIndex = m_queueFamily.GetIndices(queueFamilyIndex);
                queueInfo.queueCount = 1;
                float queuePriorities = 1.0f;
                queueInfo.pQueuePriorities = &queuePriorities;

                queueInfoList.push_back( queueInfo );
            }
        }

        // query for supported device feature , we are passing all 
        VkPhysicalDeviceFeatures supportedDeviceFeatures = {};
        vkGetPhysicalDeviceFeatures( m_device.physicalDevice, &supportedDeviceFeatures );

        // query for extension needed 
        bool supported = IsSupportedDeviceExtension( m_device.physicalDevice );

        VkDeviceCreateInfo devCrInfo = {};
        devCrInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        devCrInfo.queueCreateInfoCount = static_cast<uint32_t> (queueInfoList.size( ));
        devCrInfo.pQueueCreateInfos = queueInfoList.data( );
        devCrInfo.pEnabledFeatures = &supportedDeviceFeatures;
        if (supported) {
            devCrInfo.enabledExtensionCount = static_cast<uint32_t> (deviceExtensionRequired.size());
            devCrInfo.ppEnabledExtensionNames = deviceExtensionRequired.data();
        }

        if (vkCreateDevice( m_device.physicalDevice, &devCrInfo, nullptr, &m_device.logicalDevice ) != VK_SUCCESS)
            throw std::runtime_error( "Logical device creation failed!" );

        for (uint32_t i = 0; i < MAX_QUEUE_FAMILY_COUNT; i++) {
            auto queueFamilyIndex = static_cast<QueueFamily> (i);
            if (m_queueFamily.isValid( queueFamilyIndex ))
                vkGetDeviceQueue( m_device.logicalDevice,queueFamilyIndex,0, &m_queueFamily.queue[i] );
        }
    }
    void VulkanRenderer::CreateSurface( )
    {
        if (m_window == nullptr)
            throw std::runtime_error( "Window handle is Null!" );
        if (glfwCreateWindowSurface( m_instance, m_window, nullptr, &m_surface ) != VK_SUCCESS)
            throw std::runtime_error( "Surface creation failed!" );
    }
    void VulkanRenderer::CreateSwapChain( )
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_device.physicalDevice, m_surface, &surfaceCapabilities );
        VkSwapchainCreateInfoKHR swCrInfo = {};

        swCrInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swCrInfo.clipped = VK_TRUE;
        swCrInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swCrInfo.imageArrayLayers = 1;
        swCrInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        swCrInfo.imageExtent = surfaceCapabilities.currentExtent;
        swCrInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
        swCrInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swCrInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swCrInfo.minImageCount = surfaceCapabilities.minImageCount + 1 ;
        swCrInfo.oldSwapchain = VK_NULL_HANDLE;
        swCrInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        swCrInfo.preTransform = surfaceCapabilities.currentTransform;
        swCrInfo.surface = m_surface;

        if (vkCreateSwapchainKHR( m_device.logicalDevice, &swCrInfo, nullptr, &m_swapchain.handle ) != VK_SUCCESS)
            throw std::runtime_error( "Swapchain creation failed!" );

        m_swapchain.format = VK_FORMAT_R8G8B8A8_UNORM;
        m_swapchain.extent = surfaceCapabilities.currentExtent;

        uint32_t swapchainImageCount = 0;
        vkGetSwapchainImagesKHR( m_device.logicalDevice, m_swapchain.handle, &swapchainImageCount, nullptr );
        if (swapchainImageCount == 0 ) {
            throw std::runtime_error( "Swapchain image count is 0 !" );
        }
        m_swapchain.imageList.resize( swapchainImageCount );
        vkGetSwapchainImagesKHR( m_device.logicalDevice, m_swapchain.handle, &swapchainImageCount, m_swapchain.imageList.data() );

        for (VkImage& image : m_swapchain.imageList) {
            m_swapchain.imageViewList.push_back( std::move ( CreateImageView( image, m_swapchain.format, m_swapchain.extent,VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT )));
        }
    }

    VkImageView VulkanRenderer::CreateImageView( VkImage& image, VkFormat& format, VkExtent2D& extent, VkImageViewType viewType, VkImageAspectFlags aspectFlags )
    {
        VkImageViewCreateInfo viewCrInfo = {};
        viewCrInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCrInfo.image = image;
        viewCrInfo.format = format;
        viewCrInfo.viewType = viewType;
        viewCrInfo.components.r = viewCrInfo.components.g = viewCrInfo.components.b = viewCrInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCrInfo.subresourceRange.aspectMask = aspectFlags;
        viewCrInfo.subresourceRange.baseMipLevel = 0;
        viewCrInfo.subresourceRange.levelCount = 1;
        viewCrInfo.subresourceRange.baseArrayLayer = 0;
        viewCrInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(m_device.logicalDevice, &viewCrInfo, nullptr, &imageView) != VK_SUCCESS ) {
            throw std::runtime_error( "View Creation failed!" );
        }
        return imageView;
    }
}
