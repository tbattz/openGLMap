# Add library
file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
add_library(utilities SHARED ${SOURCES})
