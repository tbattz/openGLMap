# cmake version
cmake_minimum_required(VERSION 3.10)
# Project Name
project(openGLMap)

# Set OpenGL Policy
set(OpenGL_GL_PREFERENCE "GLVND")

# Add c11++ flag to compiler
list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wno-narrowing")

# Add Cmake Module Path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

# Add Include Directory
if(WIN32)
    include_directories("${CMAKE_SOURCE_DIR}/Includes/")
    include_directories("${CMAKE_SOURCE_DIR}/Includes/freetype")
elseif(UNIX)
    include_directories(/usr/local/include/)
    include_directories(/usr/include/)
    include_directories(/usr/local/include/freetype/)
    include_directories(${CMAKE_SOURCE_DIR}/Includes/)
endif(WIN32)

if(WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/Includes)
elseif(UNIX)
    link_directories(/usr/local/lib /usr/lib /usr/local)
    link_directories(${CMAKE_SOURCE_DIR}/Includes)
endif(WIN32)

# Find Packages
find_package(GLM REQUIRED)
message(STATUS "GLM found: ${GLM_INCLUDE_DIR}")
find_package(FREETYPE REQUIRED)
MESSAGE(STATUS "Found FREETYPE: ${FREETYPE_LIBRARY}")
find_package(SOIL REQUIRED)
# message(STATUS "SOIL found: ${SOIL_LIBRARY}")
find_package(ASSIMP REQUIRED)
# message(STATUS "Assimp found: ${ASSIMP_LIBRARY}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found Glfw3: ${GLFW3_LIBRARY}")
find_package(Boost 1.55 COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
find_package(CURL REQUIRED)
if(WIN32)
    set(FREETYPE_LIBRARY "${CMAKE_SOURCE_DIR}/../Lib/freetype.a")
ENDIF(WIN32)

# Set Flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
find_package(OpenGL REQUIRED)
add_definitions(${OPENGL_DEFINITIONS})
find_package(X11 REQUIRED)
if(WIN32)
    set(LIBS ${FREETYPE_LIBRARY} ${SOIL_LIBRARY} ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} z opengl32 ${Boost_LIBRARIES} )
elseif(UNIX)
    include_directories(${Boost_INCLUDE_DIR})
    set(LIBS ${Boost_LIBRARIES} ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread SOIL assimp freetype curl)
endif(WIN32)

# Stop conflicting order of GLAD and GLFW
add_definitions(-DGLFW_INCLUDE_NONE)

# Add include files to search path for the shared libraries
include_directories(src)

# Build the subdirectories
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(third_party/google-test)
# Build the subdirectories
add_subdirectory(src/userInput)
add_subdirectory(src/objects/externalInput)
add_subdirectory(src/renderEngine)
add_subdirectory(src/objects)
add_subdirectory(src/utilities)
# Add the submodule to be built
#add_subdirectory(openGLPlotLive/src)

