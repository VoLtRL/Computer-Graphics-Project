// shape.cpp

#include "shape.h"

Shape::Shape(Shader *shader_program) : shader_program_(shader_program->get_id()),
                                       color(1.0f, 1.0f, 1.0f),
                                       useCheckerboard(false),
                                       isEmissive(false)
{
}

void Shape::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) {
    glUseProgram(this->shader_program_);
    
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    glUniform3f(glGetUniformLocation(this->shader_program_, "objectColor"), color.x, color.y, color.z);
    glUniform1i(glGetUniformLocation(this->shader_program_, "useCheckerboard"), useCheckerboard);
    glUniform1i(glGetUniformLocation(this->shader_program_, "isEmissive"), isEmissive);
    glUniformMatrix3fv(glGetUniformLocation(this->shader_program_, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform1f(glGetUniformLocation(this->shader_program_, "alpha"), alpha);
    
    GLint loc = glGetUniformLocation(this->shader_program_, "model");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));
    
    loc = glGetUniformLocation(this->shader_program_, "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

    loc = glGetUniformLocation(this->shader_program_, "projection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));
}