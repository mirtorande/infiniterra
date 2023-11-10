#ifndef CUBEMAP_MANAGER_H
#define CUBEMAP_MANAGER_H

#include <string>
#include <glad/glad.h>
#include <vector>
#include <iostream>
#include "stb_image.h"

class Cubemap {
public:
    Cubemap();
    Cubemap(const std::vector<std::string>& faces);
    ~Cubemap();

    GLuint loadCubemap(const std::vector<std::string>& faces);
    void bindCubemap(); // Bind the texture associated with this TextureManager object

private:
    GLuint cubemapID; // Member variable to store the ID of the loaded texture
};

#endif // CUBEMAP_MANAGER_H
