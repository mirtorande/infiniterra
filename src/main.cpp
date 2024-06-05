#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#include "stb_image.h"
#include "terrain_generation.h"
#include "terrain.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"


#include <shader.h>
#include <camera.h>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


// settings
const unsigned SCR_WIDTH = 2000;
const unsigned SCR_HEIGHT = 1000;
const unsigned CHUNK_SIZE = 1000;
const unsigned TERRAIN_RESOLUTION = 20;
const int VIEW_DISTANCE = 6; //In chunks

// camera - give pretty starting point
Camera camera(glm::vec3(0.0f, 80.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "InfiniTerra by Mirto", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLint maxTessLevel;
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
    std::cout << "Max available tess level: " << maxTessLevel << std::endl;

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    

    // Generate Chunk
    std::vector<float> terrainGeometry = TerrainGenerator::GenerateChunkGeometry(CHUNK_SIZE, CHUNK_SIZE, TERRAIN_RESOLUTION);
  
    std::unordered_map<glm::vec2, Terrain*> chunks;

    // Imgui

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    // ImGUI Variables
    static bool wireframe = false;
    static float terrainHeight = 256.0f;
    static int terrainResolution = 20;
    static float terrainFrequency = 10.0f;
    static int minTess = 1;
    static int maxTess = 64;
    static float minDist = 40.0f;
    static float maxDist = 1600.0f;
    static float fogDist = 5000.0f;
    static float grassColor[3] = { 0.2f, 0.3f, 0.1f };
    static float dryColor[3] = { 0.5f, 0.4f, 0.3f };
    static float snowColor[3] = { 1.0f, 1.0f, 1.0f };
    static float skyColor[3] = { 0.6f, 0.6f, 0.7f };
    
    #define NAV_SETTINGS_HEIGHT 120
    #define TER_SETTINGS_HEIGHT 360
    #define GEN_SETTINGS_HEIGHT 100
    #define CLOSED_SETTINGS_HEIGHT 20
    #define SETTINGS_WIDTH 400
    #define SETTINGS_X 20
    #define SETTINGS_Y 20
    #define SETTINGS_SPACING 5

    static bool navSettingsCollapsed = false;
    static bool terSettingsCollapsed = false;
    static float fps[100];
    static int fpsIndex = 0;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        // 
        //convert colors to glm::vec3
        glm::vec3 grassColorVec = glm::vec3(grassColor[0], grassColor[1], grassColor[2]);
        glm::vec3 dryColorVec = glm::vec3(dryColor[0], dryColor[1], dryColor[2]);
        glm::vec3 snowColorVec = glm::vec3(snowColor[0], snowColor[1], snowColor[2]);
        glm::vec3 skyColorVec = glm::vec3(skyColor[0], skyColor[1], skyColor[2]);

        glClearColor(skyColor[0], skyColor[1], skyColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();            
        ImGui::NewFrame();


        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, fogDist);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        // Render the chunks
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);


        // Find the center of the chunk the camera is in
        float chunkCenterX = floor(camera.Position.x / CHUNK_SIZE);
        float chunkCenterZ = floor(camera.Position.z / CHUNK_SIZE);
      
        for (int z = -VIEW_DISTANCE; z < VIEW_DISTANCE; z++)
        {
            for (int x = -VIEW_DISTANCE; x < VIEW_DISTANCE; x++)
            {
                glm::vec2 chunkPos = glm::vec2((chunkCenterX + x) * CHUNK_SIZE, (chunkCenterZ + z) * CHUNK_SIZE);
                if (chunks.count(chunkPos) == 0)
                {
                    // Generate the chunk
                    chunks.insert(std::pair(chunkPos, new Terrain((chunkCenterX + x) * CHUNK_SIZE, (chunkCenterZ + z) * CHUNK_SIZE, CHUNK_SIZE, terrainResolution, terrainFrequency, terrainGeometry)));
				}
                // Render the chunk if it is within the view cone
                glm::vec2 chunkDirection = chunkPos - glm::vec2(camera.Position.x, camera.Position.z);
                float distance = glm::length(chunkDirection);
                if (distance < CHUNK_SIZE || glm::dot(glm::vec2(camera.Front.x, camera.Front.z), chunkDirection) > 0.99f) {
                    chunks[chunkPos]->Render(
                        glm::translate(model, glm::vec3(chunkPos.x, 0.0f, chunkPos.y)), view, projection,
                        terrainHeight, camera.Position,
                        minTess, maxTess, minDist, maxDist, fogDist,
                        grassColorVec, dryColorVec, snowColorVec
                    );
                }
            }
        }

        ImGui::Begin("Navigation Settings");
        ImGui::SetWindowPos(ImVec2(SETTINGS_X, SETTINGS_Y));
        ImGui::SetWindowSize(ImVec2(SETTINGS_WIDTH, NAV_SETTINGS_HEIGHT));
        ImGui::Text("FPS: %.2f", 1.0f / deltaTime);
        //compute and display average FPS
        
        fps[fpsIndex] = 1.0f / deltaTime;
        fpsIndex = (fpsIndex + 1) % 100;
        float avgFps = 0.0f;
        // Add some space
        ImGui::Spacing();
        // Add text to display the camera position shortened to 2 decimal places
        ImGui::Text("Camera position:\n x:%.2f y:%.2f z:%.2f", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Spacing();
        // Add option to capture mouse cursor
        if (ImGui::Button("Capture Mouse (space to free)")) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
        // React to window collapsing
        navSettingsCollapsed = ImGui::IsWindowCollapsed();
        ImGui::End();

        ImGui::Begin("Terrain Settings");
        //make it so that this window cannot be dragged
        int terSettingsY = SETTINGS_Y + NAV_SETTINGS_HEIGHT * !navSettingsCollapsed + CLOSED_SETTINGS_HEIGHT * navSettingsCollapsed + SETTINGS_SPACING;
        ImGui::SetWindowPos(ImVec2(SETTINGS_X, terSettingsY));
        ImGui::SetWindowSize(ImVec2(SETTINGS_WIDTH, TER_SETTINGS_HEIGHT));
        // Tickbox to enable/disable wireframe mode
        ImGui::Checkbox(" Wireframe view:", &wireframe);
        ImGui::SameLine();
        ImGui::Text("%s", wireframe ? "on" : "off");
        ImGui::Spacing();
        // Terrain height slider
        ImGui::SliderFloat("Terrain Height", &terrainHeight, 0.0f, 1000.0f);
        // Terrain tessellation settings
        ImGui::SliderInt("Min Tess Level", &minTess, 1, maxTessLevel);
        ImGui::SliderInt("Max Tess Level", &maxTess, 1, maxTessLevel);
        ImGui::SliderFloat("Min Tess Distance", &minDist, 1, 5000);
        ImGui::SliderFloat("Max Tess Distance", &maxDist, 1, 5000);
        // Terrain fog distance slider
        ImGui::SliderFloat("Fog Distance", &fogDist, 1, 10000);
        // Color edit for the terrain and the sky
        ImGui::Text("Terrain Colors");
        ImGui::ColorEdit3("Grass Color", grassColor);
        ImGui::ColorEdit3("Dry Color", dryColor);
        ImGui::ColorEdit3("Snow Color", snowColor);
        ImGui::Text("Sky Color");
        ImGui::ColorEdit3("Sky Color", skyColor);
        // React to window collapsing
        terSettingsCollapsed = ImGui::IsWindowCollapsed();
        ImGui::End();

        ImGui::Begin("Generation Settings (Requires rebuild)");
        // Create this window under the first two
        int genSettingsY = SETTINGS_Y + SETTINGS_SPACING + NAV_SETTINGS_HEIGHT * !navSettingsCollapsed + CLOSED_SETTINGS_HEIGHT * navSettingsCollapsed + SETTINGS_SPACING + TER_SETTINGS_HEIGHT * !terSettingsCollapsed + CLOSED_SETTINGS_HEIGHT * terSettingsCollapsed;
        ImGui::SetWindowPos(ImVec2(SETTINGS_X, genSettingsY));
		ImGui::SetWindowSize(ImVec2(SETTINGS_WIDTH, GEN_SETTINGS_HEIGHT));
        // Terrain resolution slider
        ImGui::SliderInt("Resolution", &terrainResolution, 1, 20);
        // Terrain frequency slider
        ImGui::SliderFloat("Frequency", &terrainFrequency, 1.0f, 50.0f);
        // Button to regenerate the terrain
        if (ImGui::Button("Regenerate Terrain")) {
			terrainGeometry = TerrainGenerator::GenerateChunkGeometry(CHUNK_SIZE, CHUNK_SIZE, terrainResolution);
            for (auto& pair : chunks) {
				delete pair.second; // Delete the Terrain object pointed to by the second element of the pair
			}
			chunks.clear();
		}
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // glfw: swap buffers and poll IO events
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
        // Free memory for all Terrain objects stored in chunks
    for (auto& pair : chunks) {
        delete pair.second; // Delete the Terrain object pointed to by the second element of the pair
    }
        // Clear the chunks map
    chunks.clear();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}




// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey (window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.MovementSpeed = 150.0f;
	else
		camera.MovementSpeed = 75.0f;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(ASCEND, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DESCEND, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever a key event occurs, this callback is called
// ---------------------------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        default:
            break;
        }
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    int mode = glfwGetInputMode(window, GLFW_CURSOR);

    // Check if the mode is as expected
    if(mode == GLFW_CURSOR_DISABLED)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}