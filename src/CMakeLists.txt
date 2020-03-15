# Add Cmake Module Path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")


# Add Sources
file(GLOB SOURCES LIST_DIRECTORIES true *.h *.cpp)
set(SOURCES ${SOURCES})

# Add an executable target
MESSAGE( STATUS "Sources: " ${SOURCES} )
add_executable(openGLMap ${SOURCES})

# Specify libraries to be linked
target_link_libraries(openGLMap ${LIBS} userInput renderEngine objects utilities)

# Create library for use in testing
add_library(openGLMap_objects STATIC ${SOURCES} userInput renderEngine objects utilities)