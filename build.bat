if not exist build mkdir build
cd build
:: --preset vcpkgtoolchain defined in CMakePresets.json
:: vcpkgtoolchain specifies vcpkg tool chain location
cmake -S ../ -B . --preset vcpkgtoolchain
::cmake -S ../ -B . -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
cmake --build .
cd ../