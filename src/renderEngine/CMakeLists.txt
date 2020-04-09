# Add library
file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
add_library(renderEngine SHARED ${SOURCES})

# Stop conflicting order of GLAD and GLFW
add_definitions(-DGLFW_INCLUDE_NONE)

# Specify libraries to be linked
target_link_libraries(renderEngine ${FREETYPE_LIBRARY} ${ASSIMP_LIBRARY})
