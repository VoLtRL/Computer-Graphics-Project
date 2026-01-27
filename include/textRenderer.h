#pragma once
#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "resourceManager.h"


struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;    // Size of glyph
    glm::ivec2   Bearing; // Offset from baseline to left/top of glyph
    unsigned int Advance; // Offset to advance to next glyph
};

class TextRenderer
{
public:
    std::map<char, Character> Characters; // holds all precompiled characters
    Shader TextShader; // shader used for text rendering
    TextRenderer(unsigned int width, unsigned int height); // constructor
    void Load(std::string font, unsigned int fontSize); // pre-compiles a list of characters from the given font
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f)); // renders a string of text using the precompiled list of characters
private:
    unsigned int VAO, VBO;
};