#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>
#include "shader.h"
#include "cubemap.h"
#include "VAO.h"

class Skybox {
public:
    Skybox();
    ~Skybox();

    void Render(const glm::mat4& view, const glm::mat4& projection);

private:
    Cubemap skyboxMap;
    VAO skyboxVAO;
    VBO skyboxVBO;
    Shader skyboxShader;
    static const std::vector<std::string>& faces;
    static float skyboxVertices[];  // Define static skybox vertices
};

#endif // SKYBOX_H
