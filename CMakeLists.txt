cmake_minimum_required(VERSION 3.16)
project(OpenGLProgram)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/")

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

add_subdirectory(external/glm)

add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

if (UNIX AND NOT APPLE)
    target_link_libraries(glad PRIVATE ${CMAKE_DL_LIBS})
endif()



file(GLOB SOURCES "src/*.cpp")

add_executable(opengl_program ${SOURCES})

target_include_directories(opengl_program PRIVATE 
    include

)

target_link_libraries(opengl_program
    PRIVATE
    glad
    glfw
    glm::glm    
    OpenGL::GL 
)

if (UNIX AND NOT APPLE)
    target_link_libraries(opengl_program PRIVATE ${CMAKE_DL_LIBS} pthread)
endif()