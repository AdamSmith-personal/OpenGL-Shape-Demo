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
void constructShapes(const std::vector<Shape>&);
void processInput(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

/* BASIC CONSTANTS */
static const int SUCCESS = 0;
static const int FAILURE = -1;

const unsigned int WINDOW_WIDTH = 1200;
const unsigned int WINDOW_HEIGHT = 700;


/* FILE PATHS */
static const char *vertexShaderPath = "../Resources/Shaders/default.vert";
static const char *fragmentShaderPath = "../Resources/Shaders/default.frag";
// Shape data files
static const char *octagonVerticesPath = "../Resources/Data/Vertices/octagon.txt";
static const char *octagonIndicesPath = "../Resources/Data/Indices/octagon.txt";
static const char *pyramidVerticesPath = "../Resources/Data/Vertices/pyramid.txt";
static const char *pyramidIndicesPath = "../Resources/Data/Indices/pyramid.txt";
static const char *cubeVerticesPath = "../Resources/Data/Vertices/cube.txt";
static const char *cubeIndicesPath = "../Resources/Data/Indices/cube.txt";
static const char *octahedronVerticesPath = "../Resources/Data/Vertices/octahedron.txt";
static const char *octahedronIndicesPath = "../Resources/Data/Indices/octahedron.txt";
static const char *icosahedronVerticesPath = "../Resources/Data/Vertices/icosahedron.txt";
static const char *icosahedronIndicesPath = "../Resources/Data/Indices/icosahedron.txt";
static const char *dodecahedronVerticesPath = "../Resources/Data/Vertices/dodecahedron.txt";
static const char *dodecahedronIndicesPath = "../Resources/Data/Indices/dodecahedron.txt";

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

static float rotationSpeed = 1.5f;
static bool autoRotate = false;
static bool isWireframe = false;
static bool faceCulling = true;
static bool antialiasing = true;


int main()
{
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_SAMPLES, 4);

   GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL", NULL, NULL);
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
      constructShapes(shapes);
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

/* Fills a vector with shape data from a file to be constructed. An improvement would be to put this in an array but
 * for now it is fine.
 */
void constructShapes(const std::vector<Shape>&)
{
   shapes.reserve(6);
   shapes.emplace_back(octagonVerticesPath,octagonIndicesPath);
   shapes.emplace_back(cubeVerticesPath, cubeIndicesPath);
   shapes.emplace_back(pyramidVerticesPath, pyramidIndicesPath);
   shapes.emplace_back(octahedronVerticesPath, octahedronIndicesPath);
   shapes.emplace_back(icosahedronVerticesPath, icosahedronIndicesPath);
   shapes.emplace_back(dodecahedronVerticesPath,dodecahedronIndicesPath);
}

void generateMatrices(Shader shaderProgram)
{
   glm::mat4 modelMatrix = glm::mat4(1.0f);
   modelMatrix = glm::scale(modelMatrix, scale);
   modelMatrix = glm::translate(modelMatrix, translation);
   // check if auto rotate has been enabled and change which matrix transformations are used based on the boolean auto rotate
   if (autoRotate)
   {
      modelMatrix = glm::rotate(modelMatrix, (float)glfwGetTime() * glm::radians(50.0f)*rotationSpeed, glm::vec3(0.5f, 1.0f, 0.0f));
   }
   else
   {
      modelMatrix = glm::rotate(modelMatrix, glm::radians(rotateX),glm::vec3(1.0f,0.0f,0.0f));
      modelMatrix = glm::rotate(modelMatrix, glm::radians(rotateY),glm::vec3(0.0f,1.0f,0.0f));
      modelMatrix = glm::rotate(modelMatrix, glm::radians(rotateZ),glm::vec3(0.0f, 0.0f,1.0f));
   }
   glm::mat4 viewMatrix = glm::mat4(1.0);
   viewMatrix = glm::translate(viewMatrix, cameraPosition);

   glm::mat4 projectionMatrix = glm::mat4(1.0);
   projectionMatrix = glm::perspective(glm::radians(fov), (GLfloat)WINDOW_WIDTH/(GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

   // send it all to the vertex shader
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
void createGUI() {
   ImGui::Begin("Settings");
   ImGui::Text("A demo showcasing some shapes.");

   if (ImGui::Button("Swap Shapes"))
   {
      currentShapeIndex = (currentShapeIndex + 1) % shapes.size();
   }
   ImGui::SameLine();
   ImGui::SameLine();
   if (ImGui::Button("Reset"))
   {
      resetParameters();
   }
   ImGui::Checkbox("Auto Rotate", &autoRotate);
   ImGui::SameLine();
   ImGui::Checkbox("Wire Frame", &isWireframe);
   if (isWireframe)
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   }
   else
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   }
   ImGui::Checkbox("Face Culling", &faceCulling);
   if (faceCulling)
   {
      glEnable(GL_CULL_FACE);
   } else
   {
      glDisable(GL_CULL_FACE);
   }
   ImGui::SameLine();
   ImGui::Checkbox("Anti-Aliasing",&antialiasing);
   if (antialiasing)
   {
      glEnable(GL_MULTISAMPLE);
   }
   else
   {
      glDisable(GL_MULTISAMPLE);
   }

   ImGui::Text("\nModel Matrix Parameters:");
   ImGui::SliderFloat3("Translation",&translation.x,-1.0f,1.0f);
   ImGui::SliderFloat3("Scale",&scale.x,0.0f,10.0f);
   if (!autoRotate)
   {
   ImGui::SliderFloat("Rotate X",&rotateX,0.0f,360.0f);
   ImGui::SliderFloat("Rotate Y",&rotateY,0.0f,360.0f);
   ImGui::SliderFloat("Rotate Z",&rotateZ,0.0f,360.0f);
   }
   else
   {
      ImGui::BeginDisabled();
      ImGui::SliderFloat("Rotate X",&rotateX,0.0f,360.0f);
      ImGui::SliderFloat("Rotate Y",&rotateY,0.0f,360.0f);
      ImGui::SliderFloat("Rotate Z",&rotateZ,0.0f,360.0f);
      ImGui::EndDisabled();
   }
   ImGui::Text("\nView Matrix Parameters:");
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
   autoRotate = false;
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
      if (key == GLFW_KEY_R)
      {
         resetParameters();
      }
      if (key == GLFW_KEY_A)
      {
         autoRotate = true;
      }
      if (key == GLFW_KEY_SPACE)
      {
         currentShapeIndex = (currentShapeIndex + 1) % shapes.size();
      }
      if (key == GLFW_KEY_W)
      {
         isWireframe = true;
         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }
      if (key == GLFW_KEY_F)
      {
         isWireframe = false;
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
      autoRotate = false;
      rotateX = rotateX + 1.0f;
   }
   if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
   {
      autoRotate = false;
      rotateX = rotateX - 1.0f;
   }
   if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
   {
      autoRotate = false;
      rotateY = rotateY + 1.0f;
   }
   if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
   {
      autoRotate = false;
      rotateY = rotateY - 1.0f;
   }
}
/* Resize the viewport if the window gets resized */
void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
   glViewport(0, 0, width, height);
}

