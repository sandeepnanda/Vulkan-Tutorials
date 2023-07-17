#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <exception>
#include <array>

namespace VulkanApp
{
	enum GPU_TYPE
	{
		INTEGRATED,
		DISCRETE,
		MAX_GPU_TYPE = -1,
	};

	enum QueueFamily
	{
		GRAPHICS,
		COMPUTE,
		TRANSFER,
		MAX_QUEUE_FAMILY_COUNT,
	};

	class VulkanRenderer
	{
	public:
		


		VulkanRenderer( GPU_TYPE gpuType = INTEGRATED, GLFWwindow* = nullptr);
		~VulkanRenderer( );

	private:
		VkInstance m_instance;
		struct
		{
			VkPhysicalDevice physicalDevice;
			VkDevice logicalDevice;
		}m_device;

		struct
		{
			std::array< int32_t, MAX_QUEUE_FAMILY_COUNT > indices = {-1, -1, -1};
			std::array< VkQueue, MAX_QUEUE_FAMILY_COUNT > queue;

			inline uint32_t GetIndices( QueueFamily family ) 
			{ 
				if (!isValid( family ))
					throw std::runtime_error( "InValid Queue index!" );
				return indices[family]; 
			}
			inline VkQueue& GetQueue( QueueFamily family ) 
			{
				if(!isValid(family ) || (queue[family] == nullptr))
					throw std::runtime_error( "InValid Queue Handle!" );
				return queue[family];
			}
			bool isValid( QueueFamily family ) { return (indices[family] >= 0); }
		}m_queueFamily;
		VkSurfaceKHR m_surface;
		struct
		{
			VkSwapchainKHR handle;
			VkFormat format;
			VkExtent2D extent;
			std::vector<VkImage> imageList;
			std::vector<VkImageView> imageViewList;
		}m_swapchain;
		GLFWwindow* m_window;

		//todo: move to another class
		void CreateVulkanInstance( );

		//to do: move these to another class
		void GetVukanPhysicalDevice( GPU_TYPE& );
		bool IsSupportedInstanceLayer( );
		bool IsSupportedDeviceExtension( VkPhysicalDevice& );
		void CreateVulkanLogicalDevice( );
		
		//todo: move it to another class
		void CreateSurface( );
		void CreateSwapChain( );

		//to do: move to another class
		VkImageView CreateImageView( VkImage&, VkFormat&, VkExtent2D&, VkImageViewType , VkImageAspectFlags );
	};
}