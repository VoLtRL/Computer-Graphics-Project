#include "crosshair.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

Crosshair::Crosshair(float size) : size(size) {
    std::string shader_dir = SHADER_DIR;
    shader_program = new Shader(shader_dir+"crosshair.vert", shader_dir+"crosshair.frag");

    float h = size / 2.0f;

    float vertices[] = {
        // First Triangle
        -h,  h, 0.0f,   0.0f, 1.0f,
        -h, -h, 0.0f,   0.0f, 0.0f,
         h, -h, 0.0f,   1.0f, 0.0f,

        // Second Triangle
        -h,  h, 0.0f,   0.0f, 1.0f,
         h, -h, 0.0f,   1.0f, 0.0f,
         h,  h, 0.0f,   1.0f, 1.0f,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Crosshair::draw(unsigned int textureID, float aspectRatio) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint id = shader_program->get_id();
    glUseProgram(id);


    // projection matrix
    glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
    // uniforms
    glUniformMatrix4fv(glGetUniformLocation(id, "projection"), 1, GL_FALSE, &projection[0][0]);
    // set texture uniform
    glUniform1i(glGetUniformLocation(id, "crosshairTexture"), 0);
    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);


    // 4. Draw
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}