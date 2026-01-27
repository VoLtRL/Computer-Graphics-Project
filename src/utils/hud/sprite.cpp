#include "sprite.h"
#include <glad/glad.h>

Sprite::Sprite(Shader* shader) : shader(shader) {
    this->initRenderData();
}

Sprite::~Sprite() {
    glDeleteVertexArrays(1, &quadVAO);
}

void Sprite::draw(unsigned int textureID, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color) {
    glUseProgram(this->shader->get_id());

    // Create transformation matrix
    glm::mat4 model = glm::mat4(1.0f);
    // Translate to position
    model = glm::translate(model, glm::vec3(position, 0.0f)); 
    // Move origin to center for rotation
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); 
    // Scale to size
    model = glm::scale(model, glm::vec3(size, 1.0f)); 

    glUniformMatrix4fv(glGetUniformLocation(this->shader->get_id(), "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3f(glGetUniformLocation(this->shader->get_id(), "color"), color.x, color.y, color.z);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Sprite::initRenderData() {
    unsigned int VBO;
    float vertices[] = { 
        // pos      // tex
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

glm::mat4 Sprite::getProjection(float aspectRatio) {
    return glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
}