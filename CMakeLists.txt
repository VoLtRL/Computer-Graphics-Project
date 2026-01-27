cmake_minimum_required(VERSION 3.16)
project(OpenGLProgram)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/")
add_compile_definitions(IMAGE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/images/")

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

add_subdirectory(external/glm)
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

add_library(stb STATIC external/stb/stb_image.cpp)
target_include_directories(stb PUBLIC external/stb)

ADD_SUBDIRECTORY(external/assimp)

add_subdirectory(external/freetype)



if (UNIX AND NOT APPLE)
    target_link_libraries(glad PRIVATE ${CMAKE_DL_LIBS})
endif()



file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp" "src/*/*/*.cpp")

add_executable(opengl_program ${SOURCES})
target_include_directories(opengl_program PRIVATE 
    include

)

target_link_libraries(opengl_program
    PRIVATE
    glad
    glfw
    assimp
    stb
    glm::glm
    OpenGL::GL
    freetype
)

if (UNIX AND NOT APPLE)
    target_link_libraries(opengl_program PRIVATE ${CMAKE_DL_LIBS} pthread)
endif()