#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "terrain_generation.h"
#include "terrain.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>

#include <iostream>
#include <vector>
#include <utility>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


// settings
const unsigned SCR_WIDTH = 1000;
const unsigned SCR_HEIGHT = 800;
const unsigned CHUNK_SIZE = 512;
const unsigned HI_RES_RESOLUTION = 20;
const unsigned LOW_RES_RESOLUTION = 15;
const unsigned VIEW_DISTANCE = CHUNK_SIZE * 5;
const unsigned HOW_MANY_CHUNKS_PER_SIDE = 25;

// camera - give pretty starting point
Camera camera(glm::vec3(0.0f, 80.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void generate_chunks_around_camera(std::vector<std::pair<glm::vec2,Terrain>>& chunks)
{
    std::vector<float> lowResGeometry = TerrainGenerator::GenerateChunkGeometry(CHUNK_SIZE, CHUNK_SIZE, LOW_RES_RESOLUTION);
    float bottomLeftX = floor(camera.Position.x / CHUNK_SIZE);
    float bottomLeftZ = floor(camera.Position.z/CHUNK_SIZE);
	// Render the 9 chunks
    for (int z = bottomLeftZ; z < bottomLeftZ+3; z++)
    {
		for (int x = bottomLeftX; x < bottomLeftX+3; x++)
		{
            chunks.emplace_back(glm::vec2(x * CHUNK_SIZE, z * CHUNK_SIZE), Terrain(x * CHUNK_SIZE, z * CHUNK_SIZE, CHUNK_SIZE, lowResGeometry, LOW_RES_RESOLUTION));
		}
	}
}

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL: Terrain GPU", NULL, NULL);
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
    int width = CHUNK_SIZE, height = CHUNK_SIZE, nrChannels = 1;
    std::vector<float> hiResGeometry = TerrainGenerator::GenerateChunkGeometry(CHUNK_SIZE, CHUNK_SIZE, HI_RES_RESOLUTION);
    std::vector<float> lowResGeometry = TerrainGenerator::GenerateChunkGeometry(CHUNK_SIZE, CHUNK_SIZE, LOW_RES_RESOLUTION);
    //std::vector<Terrain> chunks;
    std::vector<std::pair<glm::vec2, Terrain>> chunks;
    chunks.reserve(100);

    
    // Make the 9 chunks square around 0,0 with two for loops with variables z and x
    float centerOffset = HOW_MANY_CHUNKS_PER_SIDE * CHUNK_SIZE / 2;
    for (int z = 0; z < HOW_MANY_CHUNKS_PER_SIDE; z++)
    {
        for (int x = 0; x < HOW_MANY_CHUNKS_PER_SIDE; x++)
        {
			chunks.emplace_back(glm::vec2(x * CHUNK_SIZE - centerOffset, z * CHUNK_SIZE - centerOffset), Terrain(x * CHUNK_SIZE, z * CHUNK_SIZE, CHUNK_SIZE, lowResGeometry, LOW_RES_RESOLUTION));
		}
	}

    //Terrain testChunk = Terrain(CHUNK_SIZE, CHUNK_SIZE, lowResGeometry, LOW_RES_RESOLUTION);

    // Imgui

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    bool notGenerated = true;
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
        glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();            
        ImGui::NewFrame();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        if (notGenerated && camera.Position.x > HOW_MANY_CHUNKS_PER_SIDE * CHUNK_SIZE)
        {
            notGenerated = false;
			//generate_chunks_around_camera(chunks);
        }

        // Render the chunks
   
        for (int i = 0; i < chunks.size(); i++)
        {
            // Render the chunk only if it's in the camera's view
            //if (camera.Position.x - VIEW_DISTANCE < chunks[i].first.x && chunks[i].first.x < camera.Position.x + VIEW_DISTANCE &&
            //    camera.Position.z - VIEW_DISTANCE < chunks[i].first.z && chunks[i].first.z < camera.Position.z + VIEW_DISTANCE)
            chunks[i].second.Render(glm::translate(model, glm::vec3(chunks[i].first.x, 0.0f, chunks[i].first.y)), view, projection, camera.Position);
        }
      
        //testChunk.Render(model, view, projection);

        ImGui::Begin("Terrain Generator");
        ImGui::SetWindowSize(ImVec2(400, 200));
        ImGui::Text("FPS: %f", 1.0f / deltaTime);
        ImGui::Text("Camera position: x:%f y:%f z:%f", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Text("Hasn't Splurged: %s", notGenerated ? "true":"false");
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

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}