cmake_minimum_required(VERSION 3.26.0)

#Set the project 
set(NAME VulkanEngine)
project(${NAME} VERSION 1.0.0 )

#Find Vulkan
find_package(Vulkan REQUIRED)
message(STATUS "Vulkan Found $ENV{VULKAN_SDK}")

#Find GLM
find_package(glm CONFIG REQUIRED)

#Find GLFW
find_package(glfw3 CONFIG REQUIRED)

#Dependencies Include directories
message(STATUS "Project source path : ${PROJECT_SOURCE_DIR}")
message(STATUS "VULKAN Sdk path     : ${Vulkan_INCLUDE_DIRS}")

file(GLOB_RECURSE SOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/*.hpp
    )
    
add_executable(${PROJECT_NAME} ${SOURCES})

#Set directories & Libs for the project
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
)

#Set Libraries 
message(STATUS "VULKAN Library : ${Vulkan_LIBRARIES}")

target_link_libraries(${PROJECT_NAME} 
    glfw 
    Vulkan::Vulkan
    glm::glm
)