#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>
#include <WindowIO.h>
#include <skybox.h>
#include <VAO.h>
#include <EBO.h>

#include <iostream>

float getHeightFromImage(const unsigned char* data, int width, int x, int y) {
    // Assuming the image is grayscale, so we take only one channel
    return static_cast<float>(data[y * width + x]) / 255.0f;
}

void generateTerrain(std::vector<float>& vertices, const unsigned char* data, int width, int height, float scale) {
    // Generate vertices for a grid
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float xPos = static_cast<float>(x) * scale;
            float yPos = getHeightFromImage(data, width, x, y) * scale;
            float zPos = static_cast<float>(y) * scale;

            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
        }
    }
}

int main()
{
    GLFWwindow* window = initializeWindow();

    // configure global opengl state
    // -----------------------------
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");
    Shader terrainShader("shaders/terrain.vert", "shaders/terrain.frag");


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    VAO skyboxVAO;
    skyboxVAO.Bind();
    VBO skyboxVBO(skyboxVertices, sizeof(skyboxVertices));
    skyboxVAO.LinkVBO(skyboxVBO, 0);
    skyboxVAO.Unbind();


    // ========================HEIGHT MAP==================================
    // Load heightmap image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true); // Flip the image vertically, as OpenGL has its origin at the bottom-left
    unsigned char* heightmapData = stbi_load("resources/textures/terrain.png", &width, &height, &channels, 0);
    if (!heightmapData) {
        std::cerr << "Failed to load heightmap image" << std::endl;
        return -1;
    }

    // Generate terrain vertices
    std::vector<float> terrainVertices;
    float scale = 0.1f; // Adjust the scale based on your preference
    generateTerrain(terrainVertices, heightmapData, width, height, scale);

    stbi_image_free(heightmapData); // Free the image data, as it's no longer needed

    VAO terrainVAO;
    terrainVAO.Bind();
    VBO terrainVBO(terrainVertices.data(), terrainVertices.size());
    terrainVAO.LinkVBO(terrainVBO, 0);
    terrainVAO.Unbind();
    // ====================================================================

    // load textures
    // -------------
    unsigned int cubemapTexture = loadCubemap(faces);

    // shader configuration
    // --------------------
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw scene as normal
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
        terrainShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        terrainShader.setMat4("model", model);
        terrainShader.setMat4("view", view);
        terrainShader.setMat4("projection", projection);
        terrainVAO.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_POINTS, 0, static_cast<int>(terrainVertices.size() / 3));
        terrainVAO.Unbind();
        /*
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        skyboxVAO.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        skyboxVAO.Unbind();
        glDepthFunc(GL_LESS); // set depth function back to default*/

        // glfw: swap buffers and poll IO events
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    skyboxVAO.Delete();
    skyboxVBO.Delete();
    glfwTerminate();
    return 0;
}