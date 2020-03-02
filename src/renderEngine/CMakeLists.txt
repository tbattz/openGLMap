# Add library
file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
add_library(renderEngine SHARED ${SOURCES})

# Specify libraries to be linked
target_link_libraries(renderEngine ${FREETYPE_LIBRARY} ${ASSIMP_LIBRARY})
