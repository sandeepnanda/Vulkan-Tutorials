## Build Procedure

This project uses Cmake for building procedures. For handling various 3rd patry dependencies vcpkg manager is being used as this tool works seamlessly with Cmake tool.

Here are step by step guide to use vcpkg manager in this project
1. Install vcpkg manager from [here](https://vcpkg.io/en/getting-started.html)
2. Add an system environment variable for the vcpkg root directory with this name 
    
        %VCPKG_ROOT% = C:\devtools\vcpkg
3. vcpkg manager can be used 2 ways 
    
    * System wide installtion of dependent packages, which installs for all users and all projects
    * Using menifest json file, which installs dependent packages only for the project. ***we are using this method*** 
4. Go to your project root folder where you have defined the base level ***CMakeLists.txt*** 
5. Create a json file with name ***vcpkg.json*** and provide dependencies for the project
    
        {
            "dependencies":[
                "glfw3",
                "glm"
            ]
        }
6. Use your depenent libs in CMakeLists.txt using ***find_package(depedent lib name)***
7. While configuring Cmake we need to provide the vcpkg root folder by cmake tool chain file, we can do it in 2 ways
        
        cmake -S ../ -B . -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake

    or

    Define Cmake preset file( ***CMakePresets.json*** ) and provide vcpkg tool chain 
    
        {
            "version": 5,
            "configurePresets": [
                {
                    "name": "vcpkgtoolchain",
                    "toolchainFile": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
                }
            ]
        }

    Use this preset  while configuring project usign Cmake
    
        cmake -S ../ -B . --preset vcpkgtoolchain