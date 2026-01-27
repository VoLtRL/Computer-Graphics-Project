#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform vec3 color;

void main()
{
    vec4 texColor = texture(image, TexCoords);

    if(texColor.a < 0.1)
        discard;
        
    FragColor = vec4(color, 1.0) * texColor;
}