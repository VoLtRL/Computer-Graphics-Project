#include "shader.h"
#include "shape.h"

#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Sphere : public Shape {
public:
public:
    Sphere(Shader *shader_program, float radius = 0.5f, int slices = 16);
    float radius;
    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection);
    virtual ~Sphere();


private:
    unsigned int num_indices;
    GLuint VAO;
    GLuint buffers[2];
};