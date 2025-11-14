/* GUI */
#include "../Header Files/imgui/imgui.h" // The GUI library itself
#include "../Header Files/imgui/imgui_impl_glfw.h" // Need this since I am using GLFW for my window
#include "../Header Files/imgui/imgui_impl_opengl3.h" // Need this since this is an OpenGL app
/* OpenGL */
#include <glad/glad.h> // glad
#include <GLFW/glfw3.h> // software to create a window to render
#include <glm/glm.hpp> // OpenGL Mathematics library (eg. matrices/mat4s, vectors/vec4s)
#include <iostream> // Basic C++ I/O
#include <vector> // C++ Vectors/Linked Lists
#include <glm/gtc/matrix_transform.hpp> // eg). contains all the different types of transformation matrices for graphics
#include <glm/gtc/type_ptr.hpp> // to get a pointer to my matrices/vectors
/*  Classes */
#include "../Header Files/ShaderClass.h" // A class to easily load shader files
#include "../Header Files/Shape.h" // A class to create shapes that get there data from a file.

/* PROTYPES */
void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void generateMatrices(Shader shaderProgram);
void initializeGUI(GLFWwindow *window);
void createGUIFrame();
void createGUI();
void deleteGUI();
void resetParameters();
void initShapes(const std::vector<Shape>&);
void processInput(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

/* BASIC CONSTANTS */
static const int SUCCESS = 0;
static const int FAILURE = -1;

const unsigned int SCREEN_WIDTH = 1000;
const unsigned int SCREEN_HEIGHT = 800;

/* FILE PATHS */
static const char *vertexShaderPath = "../Resources/Shaders/default.vert";
static const char *fragmentShaderPath = "../Resources/Shaders/default.frag";
/* SHAPE DATA PATHS */
static const char *pyramidVerticesPath = "../Resources/Data/Vertices/pyramid.txt";
static const char *pyramidIndicesPath = "../Resources/Data/Indices/pyramid.txt";
static const char *cubeVerticesPath = "../Resources/Data/Vertices/cube.txt";
static const char *cubeIndicesPath = "../Resources/Data/Indices/cube.txt";
static const char *octahedronVerticesPath = "../Resources/Data/Vertices/octahedron.txt";
static const char *octahedronIndicesPath = "../Resources/Data/Indices/octahedron.txt";
static const char *octagonVerticesPath = "../Resources/Data/Vertices/octagon.txt";
static const char *octagonIndicesPath = "../Resources/Data/Indices/octagon.txt";

/* Parameters */
static float rotateX = 0.0f;
static float rotateY = 0.0f;
static float rotateZ = 0.0f;
static float fov = 45.0f;
static glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
static glm::vec3 scale = glm::vec3(0.6f, 0.6f, 0.6f);
static glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -2.2f);

static std::vector<Shape> shapes;
static int currentShapeIndex = 0;

int main()
{
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", NULL, NULL);
   if (window == NULL)
   {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return FAILURE;
   }
   glfwMakeContextCurrent(window);
   glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
   {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return FAILURE;
   }
   Shader shaderProgram(vertexShaderPath, fragmentShaderPath);
   initializeGUI(window);

   while(!glfwWindowShouldClose(window))
   {
      initShapes(shapes);
      generateMatrices(shaderProgram);
      processInput(window);
      createGUIFrame();
      // clear the window color every frame
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      // enable depth testing
      glEnable(GL_DEPTH_TEST);
      // clear the color and depth buffers before each render iteration
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      shaderProgram.Activate();
      shapes[currentShapeIndex].Draw();
      createGUI();

      glfwSwapBuffers(window);
      glfwPollEvents();
   }
   deleteGUI();
   shaderProgram.Delete();
   for (int i = 0; i < shapes.size(); i++)
   {
      shapes[i].Delete();
   }
   glfwTerminate();
   return SUCCESS;
}

void initShapes(const std::vector<Shape>&)
{
   shapes.emplace_back(cubeVerticesPath, cubeIndicesPath);
   shapes.emplace_back(pyramidVerticesPath, pyramidIndicesPath);
   shapes.emplace_back(octahedronVerticesPath, octahedronIndicesPath);
   shapes.emplace_back(octagonVerticesPath, octagonIndicesPath);
}

void generateMatrices(Shader shaderProgram)
{
   glm::mat4 modelMatrix = glm::mat4(1.0f);
   modelMatrix = glm::scale(modelMatrix, scale);
   modelMatrix = glm::translate(modelMatrix, translation);
   modelMatrix = glm::rotate(modelMatrix, glm::radians(rotateX),glm::vec3(1.0f,0.0f,0.0f));
   modelMatrix = glm::rotate(modelMatrix, glm::radians(rotateY),glm::vec3(0.0f,1.0f,0.0f));
   modelMatrix = glm::rotate(modelMatrix, glm::radians(rotateZ),glm::vec3(0.0f, 0.0f,1.0f));

   glm::mat4 viewMatrix = glm::mat4(1.0);
   viewMatrix = glm::translate(viewMatrix, cameraPosition);

   glm::mat4 projectionMatrix;
   projectionMatrix = glm::perspective(glm::radians(fov), (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1f, 100.0f);

   int modelLocation = glGetUniformLocation(shaderProgram.ID, "modelMatrix");
   glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

   int viewLocation = glGetUniformLocation(shaderProgram.ID, "viewMatrix");
   glUniformMatrix4fv(viewLocation, 1, GL_FALSE,glm::value_ptr(viewMatrix));

   int projectionLocation = glGetUniformLocation(shaderProgram.ID, "projectionMatrix");
   glUniformMatrix4fv(projectionLocation, 1, GL_FALSE,glm::value_ptr(projectionMatrix));
}

void initializeGUI(GLFWwindow *window)
{
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO &io = ImGui::GetIO(); (void)io;
   ImGui::StyleColorsDark();
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init("#version 460");
}

void createGUIFrame()
{
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();
}

/*
 * A function to hold all the GUI boilerplate code
 */
void createGUI()
{
   ImGui::Begin("Settings");
   ImGui::Text("A demo showcasing some shapes.");

   if (ImGui::Button("Swap Shapes"))
   {
      currentShapeIndex = (currentShapeIndex + 1) % shapes.size();
   }
   ImGui::SameLine();
   if (ImGui::Button("Wireframe"))
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   }
   ImGui::SameLine();
   if (ImGui::Button("Fill Shapes"))
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   }
   ImGui::SameLine();
   if (ImGui::Button("Reset"))
   {
      resetParameters();
   }
   ImGui::Text("\nModel Matrix Parameters:");
   ImGui::SliderFloat3("Translation",&translation.x,-1.0f,1.0f);
   ImGui::SliderFloat3("Scale",&scale.x,0.0f,10.0f);
   ImGui::SliderFloat("Rotate X",&rotateX,0.0f,360.0f);
   ImGui::SliderFloat("Rotate Y",&rotateY,0.0f,360.0f);
   ImGui::SliderFloat("Rotate Z",&rotateZ,0.0f,360.0f);
   ImGui::Text("\nView Matrix Parameters:");
   // TODO: Read Camera chapter
   ImGui::SliderFloat3("Camera Position", &cameraPosition.x,-10.0f,10.0f);
   ImGui::Text("\nProjection Matrix Parameters:");
   ImGui::SliderFloat("FOV",&fov,0.0f,180.0f);
   ImGui::End();

   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
/* Function for deleting the GUI */
void deleteGUI()
{
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();
}
/* Function which resets parameters */
void resetParameters()
{
   rotateX = 0.0f;
   rotateY = 0.0f;
   rotateZ = 0.0f;
   fov = 45.0f;
   translation = glm::vec3(0.0f, 0.0f, 0.0f);
   scale = glm::vec3(0.6f, 0.6f, 0.6f);
   cameraPosition = glm::vec3(0.0f, 0.0f, -2.2f);
}

/* This is used for buttons that are only pressed once and don't constantly update stuff*/
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
   if (action == GLFW_PRESS)
   {
      if (key == GLFW_KEY_SPACE)
      {
         currentShapeIndex = (currentShapeIndex + 1) % shapes.size();
      }
      if (key == GLFW_KEY_W)
      {
         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }
      if (key == GLFW_KEY_F)
      {
         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }
      if (key == GLFW_KEY_ESCAPE)
      {
         glfwSetWindowShouldClose(window, true);
      }
   }
}
/* If you hold down these button presses it constantly updates stuff */
void processInput(GLFWwindow *window)
{
   glfwSetKeyCallback(window, keyCallback);
   if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
   {
      rotateX = rotateX + 1.0f;
   }
   if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
   {
      rotateX = rotateX - 1.0f;
   }
   if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
   {
      rotateY = rotateY + 1.0f;
   }
   if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
   {
      rotateY = rotateY - 1.0f;
   }
}
/* Resize the viewport if the window gets resized */
void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
   glViewport(0, 0, width, height);
}

