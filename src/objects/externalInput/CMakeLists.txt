# Add library
file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
add_library(externalInput SHARED ${SOURCES})
