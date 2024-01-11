#include "terrain.h"

const unsigned int NUM_PATCH_PTS = 4;


Terrain::Terrain(const int width, const int height, std::vector<float> vertices, static unsigned char* heights, unsigned res) :
    tessHeightMapShader("shaders/terrain.vs", "shaders/terrain.fs", "shaders/terrain.tcs", "shaders/terrain.tes"),
    tessHeightMapWireShader("shaders/terrain.vs", "shaders/wireframe.fs", "shaders/terrain.tcs", "shaders/terrain.tes")
{

    resolution = res;

    // load and create a texture
    // -------------------------
    glGenTextures(1, &heightsTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightsTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (heights)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, heights);
        glGenerateMipmap(GL_TEXTURE_2D);

        tessHeightMapShader.setInt("heightMap", 0);
        tessHeightMapWireShader.setInt("heightMap", 0);
        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, 0); // unbind texture when done so we won't accidentily mess up our texture.

    // configure the cube's VAO (and terrainVBO)
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
}

Terrain::~Terrain() {
    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
}

void Terrain::Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    // be sure to activate shader when setting uniforms/drawing objects
    tessHeightMapShader.use();


    // view/projection transformations

    tessHeightMapShader.setMat4("projection", projection);
    tessHeightMapShader.setMat4("view", view);

    // world transformation
    tessHeightMapShader.setMat4("model", model);


    // render the terrain
    glBindTexture(GL_TEXTURE_2D, heightsTexture);
    glBindVertexArray(terrainVAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * resolution * resolution);

    // Wireframe
    /*
    tessHeightMapWireShader.use();
    tessHeightMapWireShader.setMat4("projection", projection);
    tessHeightMapShader.setMat4("view", view);
    tessHeightMapShader.setMat4("model", model);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * resolution * resolution);*/
}
