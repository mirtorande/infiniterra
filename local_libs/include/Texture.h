#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <string>
#include <glad/glad.h>
#include <vector>
#include <iostream>
#include "stb_image.h"

class Texture {
public:
    Texture();
    ~Texture();

    void loadTexture(const std::string& filePath);
    void bindTexture(); // Bind the texture associated with this TextureManager object

private:
    GLuint textureID; // Member variable to store the ID of the loaded texture
};

#endif // TEXTURE_MANAGER_H
