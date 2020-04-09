# Define test executable bane
set(BINARY ${CMAKE_PROJECT_NAME}Tests)

# Find source files
file(GLOB TEST_SOURCES LIST_DIRECTORIES true *.h *.cpp)
set(SOURCES ${TEST_SOURCES} ${CMAKE_SOURCE_DIR}/Includes/glad.c)

# Add an executable target
MESSAGE( STATUS "Test Sources: " ${TEST_SOURCES})
add_executable(${BINARY} ${TEST_SOURCES})

# Specify libraries to be linked
target_link_libraries(openGLMapTests PUBLIC gtest openGLMap_objects ${LIBS} userInput renderEngine objects utilities)

# Stop conflicting order of GLAD and GLFW
add_definitions(-DGLFW_INCLUDE_NONE)

# Add tests to be run
add_test(NAME ${BINARY} COMMAND ${BINARY})
