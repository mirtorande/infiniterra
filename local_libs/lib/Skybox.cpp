#include "Skybox.h"

const std::vector<std::string>& Skybox::faces = {
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/front.jpg",
        "resources/textures/skybox/back.jpg"
};

float Skybox::skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,

    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f
    - 1.0f, -1.0f,  1.0f,
};


Skybox::Skybox() : skyboxMap(faces), skyboxVBO(skyboxVertices, sizeof(skyboxVertices)), skyboxShader("shaders/skybox.vert", "shaders/skybox.frag")
{
    skyboxVAO.Bind();
    skyboxVAO.LinkVBO(skyboxVBO, 0);
    skyboxVAO.Unbind();

    skyboxMap.loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
}


Skybox::~Skybox() {
    skyboxVAO.Delete();
    skyboxVBO.Delete();
}

void Skybox::Render(const glm::mat4& view, const glm::mat4& projection)
{
    glDepthFunc(GL_LEQUAL);
    skyboxMap.bindCubemap();
    skyboxVAO.Bind();

    skyboxShader.use();
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    skyboxVAO.Unbind();
    glDepthFunc(GL_LESS);
}

