cmake_minimum_required(VERSION 3.10)
project(OpenGLProgram)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)

add_compile_definitions(SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/")

# On compile GLFW depuis le dossier
add_subdirectory(external/glfw)

# On inclut GLM (c'est une librairie de headers, souvent pas besoin de compiler)
add_subdirectory(external/glm)

# On compile GLEW
add_subdirectory(external/glew-cmake) 

file(GLOB SOURCES "src/*.cpp")

add_executable(opengl_program ${SOURCES})

target_include_directories(opengl_program PRIVATE include)


target_link_libraries(opengl_program
    PRIVATE
    glfw             # La cible définie par external/glfw
    libglew_static   # La cible définie par glew 
    glm::glm         # La cible définie par glm
    OpenGL::GL       # Le driver OpenGL de la carte graphique
)

# Configuration Linux (toujours nécessaire si tu bouges le projet)
if (UNIX AND NOT APPLE)
    target_link_libraries(opengl_program PRIVATE ${CMAKE_DL_LIBS} pthread)
endif()