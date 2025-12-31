#version 330 core

// input attribute variable, given per vertex
in vec3 position;

// global matrix variables
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// color given to the fragment shader
out vec3 fragColor;

void main() {
    // On passe la position locale comme couleur (utile pour le debug)
    fragColor = position * vec3(2,2,2);

    gl_Position = projection * view * model * vec4(position, 1.0);
}