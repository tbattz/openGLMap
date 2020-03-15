# Add library
file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
add_library(objects SHARED ${SOURCES})

# Specify libraries to be linked
target_link_libraries(objects curl ${LIBS})
