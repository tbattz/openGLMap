# cmake version
cmake_minimum_required(VERSION 3.5)
# Project Name
project(openGLRace)

# Add c11++ flag to compiler
list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

# Add Cmake Module Path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

# Add Include Directory
include_directories(/usr/local/include/)
include_directories(/usr/local/include/freetype/)

# Add Sources
file(GLOB SOURCES "src/*.cpp" "src/*.h")

# Find Packages
find_package(ASSIMP REQUIRED)
message(STATUS "Assimp found: ${ASSIMP_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Glfw3 found: ${GLFW3_INCLUDE_DIR}")
find_package(GLEW REQUIRED)
message(STATUS "Glew found: ${GLEW_INCLUDE_DIR}")
find_package(GLM REQUIRED)
message(STATUS "GLM found: ${GLM_INCLUDE_DIR}")
find_package(SOIL REQUIRED)
message(STATUS "SOIL found: ${SOIL_INCLUDE_DIR}")
find_package(FREETYPE REQUIRED)
message(STATUS "FreeType found: ${FREETYPE_INCLUDE_DIR}")

# Set Flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
find_package(OpenGL REQUIRED)
add_definitions(${OPENGL_DEFINITIONS})
find_package(X11 REQUIRED)
set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread GLEW SOIL assimp freetype)

link_directories(/usr/local/lib /usr/lib /usr/local)

# Define executables
MESSAGE( STATUS "variable is: " ${SOURCES} )
add_executable(openGLRace ${SOURCES})
target_link_libraries(openGLRace ${LIBS})


