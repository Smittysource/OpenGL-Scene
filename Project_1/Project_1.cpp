/* Module_6_Milestone.cpp : This file contains the code necessary to represent
 *      a complex 3D object, a not so complex 3D object, a simple 3D object, and 
 *      three planes in OpenGL. All objects have been textured.
 *
 *Author:      David Smith
 *Course:      CS-320
 *Instructor:  E. Rodriguez
 *Date:        August 15, 2021
 *Version:     5.0
 */

#include <iostream>
#include <GL\glew.h>
#include <glfw\glfw3.h>
#include <vector>
#include <string>
#include "cylinder.h"
#include "Cuboid.h"
#include "camera.h"
#include "Sphere.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
    const char* const WINDOW_TITLE = "David Smith_Project 1";

    // Constants for dimensions of the window
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    const int FLOOR_LENGTH = 2;
    const int FLOOR_WIDTH = 3;
    const float WALL_LENGTH = 0.4f;
    const int WALL_WIDTH = 3;
    const float PI = 3.14159265359f;		// PI rounded

    // Type of shader resource
    enum Resource { VERTEX, FRAGMENT, PROGRAM };
    
    using std::cout;
    using std::endl;
    using std::cerr;
    using std::string;
    using std::vector;

    // Time keeping
    float deltaTime = 0.0f;     // Time between this frame and the one before it
    float lastFrame = 0.0f;     // Time of last frame

    // Mouse control variables
    float lastX = WINDOW_WIDTH / 2.0f;
    float lastY = WINDOW_HEIGHT / 2.0f;

    // Camera position information
    Camera camera(glm::vec3(0.0f, 5.0f, 5.0f));

    // Object and light data
    glm::vec3 gObjectColor(1.0f, 0.2f, 0.0f);           // Object color for shader
    glm::vec3 gLight1Color(1.0f, 1.0f, 0.941f);         // Key light color
    glm::vec3 gLight2Color(1.0f, 1.0f, 0.778f);         // Fill light color
    glm::vec3 gLight1Position(-5.1f, 6.5f, -8.3f);      // Key light position
    glm::vec3 gLight1Scale(0.3f);                       // Key light scale
    glm::vec3 gLight2Position(15.0f, 12.0f, 0.0f);      // Fill light position
    glm::vec3 gLight2Scale(1.0f);                       // Fill light scale
    GLfloat gLight1Intensity = 0.1f;                    // Intensity of lamp light
    GLfloat gLight2Intensity = 0.2f;                   // Intensity of fluorescent light
    glm::vec2 gUVScale(1.0f, 1.0f);                     // Scale for texture coordinates

    // Projection set to perspective or not
    bool perspective = true;

    // Determine if mouse entered focus for the first time
    bool firstMouse = true;

    // Multipliers for ortho parameters. Used with camera zoom and z parameters
    float orthoMinMultiplier = -10;
    float orthoMaxMultiplier = 10;

};

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif


// Structure to store mesh data
struct GLMesh {
    vector<GLfloat> vertices;   // Vertex data
    vector<GLushort> indices;   // Index data
    GLuint vao;                 // Vertex Array Object
    GLuint vbos[2];             // Vertex Buffer Objects
    GLuint nIndices;            // Number of indices
    GLuint texture;             // Texture for mesh
    glm::mat4 model;                 // Model matrix for object
};

// Structure to store light mesh data
struct GLLightMesh {
    vector<GLfloat> vertices;   // Vertex data
    vector<GLushort> indices;   // Index data
    GLuint vao;                 // Vertex array object
    GLuint vbos[2];             // Vertex buffer objects
    GLuint nIndices;            // Number of indices
    glm::mat4 model;            // Model matrix for object
};

// Vertex information
struct Vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

// GLFW Window
GLFWwindow* gWindow = nullptr;
// Mesh data
GLMesh gSoccerBall;
GLMesh gFloor;
GLMesh gWallTop;
GLMesh gWallBottom;
GLMesh gLight1;
GLMesh gLight2;

vector<GLMesh> gCoffeeTable;
vector<GLMesh> gTrim;
vector<GLMesh> gEndTable;
vector<GLMesh> gCouch;
vector<GLMesh> gLamp;

// Program for shader
GLuint gProgram1;
GLuint gProgram2;
// Texture storage
GLuint gEndTableCylindersTexture;           // Texture for legs and supports
GLuint gEndTableSurfacesTexture;            // Texture for surfaces
GLuint gCoffeeTableTopTexture;              // Texture for top surface of coffee table
GLuint gCoffeeTableUnder;                   // Texture for under sides of coffee table
GLuint gTrimTexture;                        // Texture for wall trim
GLuint gCouchTexture;                       // Texture for the couch
GLuint gCouchCushionTexture;                // Texture for the couch cushions
GLuint gLampTexture;                        // Texture for the lamp
GLuint gLampShadeTexture;                   // Texture for the lamp shade




bool Setup(int, char* [], GLFWwindow** window);
void ChangeSize(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);
void DestroyMesh(GLMesh& mesh);
void Display();
bool CreateShaderProgram(const char* VertexShaderSource, const char* fragmentShaderSource, GLuint& programId);
void DestroyShaderProgram(GLuint programId);
bool TestResource(GLuint input, Resource resource);
void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void CreatePlane(GLMesh& plane, Vertex backLeft, GLfloat length, GLfloat width, GLuint texture);
void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
void BuildObjects();
void PlaceObjects();
void LoadTexture(GLuint& texture, string filename, GLuint textureNum);
void DestroyTextures();
void CreateEndTable(vector<GLMesh>& meshArray);
void CreateVAOS(GLMesh& mesh);
void CreateCoffeeTable(vector<GLMesh>& meshArray);
void CreateCouch(vector<GLMesh>& meshArray);
void CreateLamp(vector<GLMesh>& meshArray);

/* Objects Vertex Shader Source Code*/
const GLchar* objectVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position;      // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal;        // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal;                      // For outgoing normals to fragment shader
out vec3 vertexFragmentPos;                 // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);     // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f));             // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal;            // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Objects Fragment Shader Source Code*/
const GLchar* objectFragmentShaderSource = GLSL(440,
in vec3 vertexNormal;               // For incoming normals
in vec3 vertexFragmentPos;          // For incoming fragment position
in vec2 vertexTextureCoordinate;


out vec4 fragmentColor;             // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 light2Color;
uniform vec3 lightPos;
uniform vec3 light2Pos;
uniform vec3 viewPosition;
uniform sampler2D uTexture;         // Useful when working with multiple textures
uniform vec2 uvScale;
uniform float specularIntensity1;   // Intensity of the key light
uniform float specularIntensity2;   // Intensity of the fill light

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components
     *Calculate for light 1
     *Calculate Ambient lighting*/
    float ambientStrength = 0.3f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal);                            // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos);  // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);             // Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor;                             // Generate diffuse light color

    //Calculate Specular lighting*/
    float highlightSize = 16.0f;                                // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);           // Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity1 * specularComponent * lightColor;

    /*Calculate for light 2
     *Calculate Ambient lighting*/
    float ambientStrength2 = 0.1f;                              // Set ambient or global lighting strength
    vec3 ambient2 = ambientStrength2 * light2Color;             // Generate ambient light color

    /*Calculate Diffuse lighting*/
    vec3 light2Direction = normalize(light2Pos - vertexFragmentPos);    // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact2 = max(dot(norm, light2Direction), 0.0);               // Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse2 = impact2 * light2Color;                              // Generate diffuse light color

    /*Calculate Specular lighting*/
    float highlightSize2 = 16.0f;                           // Set specular highlight size
    vec3 reflectDir2 = reflect(-light2Direction, norm);     // Calculate reflection vector
    //Calculate specular component
    float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize2);
    vec3 specular2 = specularIntensity2 * specularComponent2 * light2Color;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Combine the results from both lights into one vec3
    vec3 fillResult = (ambient + diffuse + specular);
    vec3 keyResult = (ambient2 + diffuse2 + specular2);
    vec3 lightingResult = fillResult + keyResult;

    // Calculate phong result
    vec3 phong = (lightingResult)*textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

/* Lamp Shader Source Code*/
const GLchar* lightVertexShaderSource = GLSL(440,
layout(location = 0) in vec3 position;      // Vertex data from Vertex Attrib Pointer 0
layout(location = 1) in vec3 normals;       // Color data from Vertex Attrib Pointer 1
layout(location = 2) in vec2 aTexCoord;     // Texture coordinates

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);


/* Fragment Shader Source Code*/
const GLchar* lightFragmentShaderSource = GLSL(440,
out vec4 fragmentColor;         // For outgoing lamp color (smaller cube) to the GPU
uniform vec4 color;

void main()
{
    fragmentColor = color;      // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);

// Begining of program execution
int main(int argc, char* argv[])
{
    // Set up OpenGL window
    if (!Setup(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create shader program
    if (!CreateShaderProgram(objectVertexShaderSource, objectFragmentShaderSource, gProgram1))
        return EXIT_FAILURE;
    if (!CreateShaderProgram(lightVertexShaderSource, lightFragmentShaderSource, gProgram2)) {
        return EXIT_FAILURE;
    }
    // Set background color to dark blue
    glClearColor(0.084f, 0.110f, 0.210f, 1.0f);

    // Display loop
    while (!glfwWindowShouldClose(gWindow)) {

        // Handle input
        ProcessInput(gWindow);

        // Render current frame
        Display();

        // Check for new events
        glfwPollEvents();
    }

    // Free mesh memory
    DestroyMesh(gSoccerBall);
    DestroyMesh(gFloor);
    DestroyMesh(gWallTop);
    DestroyMesh(gWallBottom);
    DestroyMesh(gLight1);
    DestroyMesh(gLight2);

    for (unsigned int i = 0; i < gCoffeeTable.size(); i++) {
        DestroyMesh(gCoffeeTable.at(i));
    }
    for (unsigned int i = 0; i < gTrim.size(); i++) {
        DestroyMesh(gTrim.at(i));;
    }
    for (unsigned int i = 0; i < gEndTable.size(); i++) {
        DestroyMesh(gEndTable.at(i));;
    }
    for (unsigned int i = 0; i < gLamp.size(); i++) {
        DestroyMesh(gLamp.at(i));;
    }
    for (unsigned int i = 0; i < gCouch.size(); i++) {
        DestroyMesh(gCouch.at(i));;
    }


    // Free shader program memmory
    DestroyShaderProgram(gProgram2);
    DestroyShaderProgram(gProgram1);

    // Exit program with success flag
    exit(EXIT_SUCCESS);

}

bool Setup(int, char* [], GLFWwindow** window) {

    // Create and initialize GLFW window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create GLFW window
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

    // Check for GLFW window creation failure
    if (*window == NULL) {
        cout << "GLFW window creation failed." << endl;
        glfwTerminate();
        return false;
    }

    // Initialize GLEW
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, ChangeSize);
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(*window, MouseScrollCallback);
    glfwSetCursorPosCallback(*window, MousePositionCallback);
    glfwSetKeyCallback(*window, KeyCallBack);
    glfwSetInputMode(*window, GLFW_STICKY_KEYS, GLFW_TRUE);

    GLenum glewInitResult = glewInit();

    // Test for GLEW initialization failure
    if (GLEW_OK != glewInitResult) {
        cerr << glewGetErrorString(glewInitResult) << endl;
        return false;
    }

    // Output OpenGL version to console
    cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;

    // Display controls
    cout << endl << "Controls:" << endl << "W moves the camera forward." << endl << "S moves the camera backwards." << endl << "A moves the camera left." << endl << "D moves the camera right." << endl
        << "Q moves the camera up." << endl << "E moves the camera down." << endl  << endl << "Scrolling the mouse wheel up will increase the speed of" << endl 
        <<"\tcamera turning with the mouse and movement with the keyboard." << endl  << endl << "Scrolling the mouse wheel down will decrease the speed of " << endl 
        << "\tcamera turning with the mouse and movement with the keyboard." << endl << endl << "This scene has smart home features. You can also use the following controls:"
        << endl << "F1 toggles the lamp between its normal color and orange." << endl << "F2 toggles the fluorescent light between its normal color and green." << endl << endl
        << "The program starts in perspective mode. P can be used to toggle between this and orthographic mode." << endl << endl;

    // Load textures
    LoadTexture(gFloor.texture, "Carpet.jpg", 0);
    LoadTexture(gWallBottom.texture, "Wall_Bottom.jpg", 1);
    LoadTexture(gWallTop.texture, "Wall_Top.jpg", 2);
    LoadTexture(gEndTableSurfacesTexture, "pxfuel.com.jpg", 3);
    LoadTexture(gEndTableCylindersTexture, "pxfuel.com_1.jpg", 4);
    LoadTexture(gCoffeeTableTopTexture, "wood-texture-plank-floor-wall-furniture-601095-pxhere.com.jpg", 5);
    LoadTexture(gCoffeeTableUnder, "90DegreeRotated-wood-texture-plank-floor-wall-furniture-601095-pxhere.com.jpg", 6);
    LoadTexture(gTrimTexture, "white-texture-paint-wallpaper-surface-blank-1371058-pxhere.com.jpg", 7);
    LoadTexture(gSoccerBall.texture, "soccer3_sph.png", 8);
    LoadTexture(gCouchCushionTexture, "PIXNIO-1952794-4076x3057.jpg", 9);
    LoadTexture(gCouchTexture, "2048px-Chenille_Fabric1.jpg", 10);
    LoadTexture(gLampTexture, "background-floor-gray-metal-metallic-smooth-1431205-pxhere.com.jpg", 11);
    LoadTexture(gLampShadeTexture, "structure-white-texture-floor-pattern-line-769994-pxhere.com.jpg", 12);

    // Build and place the objects in the scene
    BuildObjects();
    PlaceObjects();
    return true;
}

// Display the meshes in the window
void Display() {
    // Time keeping
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Enable z-depth to determine which fragments to show
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    // Clear the screen
    glClearColor(0.084f, 0.110f, 0.210f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the shader
    glUseProgram(gProgram1);

    // Get the camera position
    glm::mat4 view = camera.GetViewMatrix();

    // If the perspective type is set to projection
    glm::mat4 projection;
    if (perspective) {
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    // If the projection type is set to ortho
    else {
        projection = glm::ortho((float)(camera.Zoom / orthoMinMultiplier), (float)(camera.Zoom / orthoMaxMultiplier), (float)(camera.Zoom / orthoMinMultiplier), (float)(camera.Zoom / orthoMaxMultiplier), (float)(orthoMinMultiplier * 3.0f), (float)(orthoMaxMultiplier * 3.0f));
    }

    // Retrieves and passes transform matrices and uniforms to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgram1, "model");
    GLint viewLoc = glGetUniformLocation(gProgram1, "view");
    GLint projLoc = glGetUniformLocation(gProgram1, "projection");
    GLint objectColorLoc = glGetUniformLocation(gProgram1, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gProgram1, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gProgram1, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gProgram1, "viewPosition");
    GLint specularIntensity1Loc = glGetUniformLocation(gProgram1, "specularIntensity1");
    GLint light2ColorLoc = glGetUniformLocation(gProgram1, "light2Color");
    GLint light2PositionLoc = glGetUniformLocation(gProgram1, "light2Pos");
    GLint specularIntensity2Loc = glGetUniformLocation(gProgram1, "specularIntensity2");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gEndTable.at(0).model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLight1Color.r, gLight1Color.g, gLight1Color.b);
    glUniform3f(lightPositionLoc, gLight1Position.x, gLight1Position.y, gLight1Position.z);
    glUniform1f(specularIntensity1Loc, gLight1Intensity);
    glUniform3f(light2ColorLoc, gLight2Color.r, gLight2Color.g, gLight2Color.b);
    glUniform3f(light2PositionLoc, gLight2Position.x, gLight2Position.y, gLight2Position.z);
    glUniform1f(specularIntensity2Loc, gLight2Intensity);

    // Send camera position to the shader
    const glm::vec3 cameraPosition = camera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    // Send texture coordinate scaling to shader
    GLint UVScaleLoc = glGetUniformLocation(gProgram1, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // Activate texture
    glActiveTexture(GL_TEXTURE0);
    
    // Draw end table
    for (unsigned int i = 0; i < gEndTable.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, gEndTable.at(i).texture);
        glBindVertexArray(gEndTable.at(i).vao);
        glDrawElements(GL_TRIANGLES, gEndTable.at(i).nIndices, GL_UNSIGNED_SHORT, NULL);
    }
        
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, gSoccerBall.texture);
    // Draw soccer ball
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gSoccerBall.model));
    // Activate the VBOs in mesh's VAO
    glBindVertexArray(gSoccerBall.vao);
    // Tell openGL to draw
    glDrawElements(GL_TRIANGLES, gSoccerBall.nIndices, GL_UNSIGNED_SHORT, NULL);

    // Draw floor
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gFloor.model));
    glBindTexture(GL_TEXTURE_2D, gFloor.texture);
    glBindVertexArray(gFloor.vao);
    glDrawElements(GL_TRIANGLES, gFloor.nIndices, GL_UNSIGNED_SHORT, NULL);

    // Draw bottom half of wall
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gWallBottom.model));
    glBindTexture(GL_TEXTURE_2D, gWallBottom.texture);
    glBindVertexArray(gWallBottom.vao);
    glDrawElements(GL_TRIANGLES, gWallBottom.nIndices, GL_UNSIGNED_SHORT, NULL);

    // Draw top half of wall
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gWallTop.model));
    glBindTexture(GL_TEXTURE_2D, gWallTop.texture);
    glBindVertexArray(gWallTop.vao);
    glDrawElements(GL_TRIANGLES, gWallTop.nIndices, GL_UNSIGNED_SHORT, NULL);

    // Draw wall trim
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gTrim.at(0).model));
    for (unsigned int i = 0; i < gTrim.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, gTrim.at(i).texture);
        glBindVertexArray(gTrim.at(i).vao);
        // Draws the object
        glDrawElements(GL_TRIANGLES, gTrim.at(i).nIndices, GL_UNSIGNED_SHORT, NULL);
    }

    // Draw coffee table
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gCoffeeTable.at(0).model));
    for (unsigned int i = 0; i < gCoffeeTable.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, gCoffeeTable.at(i).texture);
        glBindVertexArray(gCoffeeTable.at(i).vao);
        // Uncomment next line to show in wireframe mode
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // Draws the object
        glDrawElements(GL_TRIANGLES, gCoffeeTable.at(i).nIndices, GL_UNSIGNED_SHORT, NULL);
    }

    // Draw couch
    for (unsigned int i = 0; i < gCouch.size(); i++) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gCouch.at(i).model));
        glBindTexture(GL_TEXTURE_2D, gCouch.at(i).texture);
        glBindVertexArray(gCouch.at(i).vao);
        // Uncomment next line to show in wireframe mode
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // Draws the object
        glDrawElements(GL_TRIANGLES, gCouch.at(i).nIndices, GL_UNSIGNED_SHORT, NULL);
    }

    // Draw lamp
    for (unsigned int i = 0; i < gLamp.size(); i++) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gLamp.at(i).model));
        glBindTexture(GL_TEXTURE_2D, gLamp.at(i).texture);
        glBindVertexArray(gLamp.at(i).vao);
        // Uncomment next line to show in wireframe mode
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // Draws the object
        glDrawElements(GL_TRIANGLES, gLamp.at(i).nIndices, GL_UNSIGNED_SHORT, NULL);
    }

    // Switch to the program for the light objects (does not interact with the lighting shaders)
    glUseProgram(gProgram2);
    modelLoc = glGetUniformLocation(gProgram2, "model");
    viewLoc = glGetUniformLocation(gProgram2, "view");
    projLoc = glGetUniformLocation(gProgram2, "projection");
    GLint colorLoc = glGetUniformLocation(gProgram2, "color");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4f(colorLoc, gLight1Color.r, gLight1Color.g, gLight1Color.b, 1.0f);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Draw light locations
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gLight1.model));
    glBindVertexArray(gLight1.vao);
    glDrawElements(GL_TRIANGLES, gLight1.nIndices, GL_UNSIGNED_SHORT, NULL);
    
    glUniform4f(colorLoc, gLight2Color.r, gLight2Color.g, gLight2Color.b, 1.0f);
    glUniformMatrix4fv(colorLoc, 1, GL_FALSE, glm::value_ptr(gLight2Color));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gLight2.model));
    glBindVertexArray(gLight2.vao);
    glDrawElements(GL_TRIANGLES, gLight2.nIndices, GL_UNSIGNED_SHORT, NULL);
    // Deactivate the VAO
    glBindVertexArray(0);
    // Swap frame buffers
    glfwSwapBuffers(gWindow);

}

// Build object meshes for the scene that don't have their own function
void BuildObjects() {

    // Create an anchor position for planes
    Vertex frontRight;
    frontRight.x = 1.0f;
    frontRight.y = 0.0f;
    frontRight.z = 1.0f;

    // Create the ball
    Sphere ball(0.5f, 12, 32, 0.0f, 0.0f, 0.0f);
    gSoccerBall.vertices = ball.GetVertices();
    gSoccerBall.indices = ball.GetIndices();
    CreateVAOS(gSoccerBall);
    
    // Create sphere for lamp light
    Sphere light1(0.5f, 12, 32, 0.0f, 0.0f, 0.0f);
    gLight1.vertices = light1.GetVertices();
    gLight1.indices = light1.GetIndices();
    CreateVAOS(gLight1);

    // Create plane for fluorescent light
    CreatePlane(gLight2, frontRight, FLOOR_LENGTH + 2, FLOOR_WIDTH, 1);
    CreateVAOS(gLight2);
    
    // Create floor and wall
    CreatePlane(gFloor, frontRight, FLOOR_LENGTH, FLOOR_WIDTH, gFloor.texture);
    CreatePlane(gWallBottom, frontRight, WALL_LENGTH, WALL_WIDTH, gWallTop.texture);
    CreatePlane(gWallTop, frontRight, WALL_LENGTH + 0.4f, WALL_WIDTH, gWallBottom.texture);
    
    // Create trim for wall
    GLMesh tempTrim;
    Cuboid lowerTrim(0.01f, 3.0f, 0.01f, 0.0f, 0.0092f, 0.0f, gTrimTexture);
    Cuboid upperTrim(0.01f, 3.0f, 0.01f, 0.0f, 0.405f, 0.0f, gTrimTexture);

    tempTrim.vertices = lowerTrim.GetVertices();
    tempTrim.indices = lowerTrim.GetIndices();
    tempTrim.texture = gTrimTexture;
    gTrim.push_back(tempTrim);

    tempTrim.vertices = upperTrim.GetVertices();
    tempTrim.indices = upperTrim.GetIndices();
    gTrim.push_back(tempTrim);
    for (unsigned int i = 0; i < gTrim.size(); i++) {
        CreateVAOS(gTrim.at(i));
    }

    // Create the lamp, end table, coffee table, and couch
    CreateLamp(gLamp);
    CreateEndTable(gEndTable);
    CreateCoffeeTable(gCoffeeTable);
    CreateCouch(gCouch);
}

// Perform matrix transformations to get model matrices for objects
void PlaceObjects() {
    // Keep the model's original dimensions
    glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
    // Set original rotation of model to 45 degree angle
    glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    // Put model in position at origin
    glm::mat4 translation = glm::translate(glm::vec3(glm::vec3(-5.1f, 2.401f, -7.0f)));

    // Model matrix
    glm::mat4 model = translation * rotation * scale;

    // Draw end table
    for (unsigned int i = 0; i < gEndTable.size(); i++) {
        gEndTable.at(i).model = model;
    }

    // Set matrix for soccer ball
    scale = glm::scale(glm::vec3(1.5f, 1.5f, 1.5f));
    translation = glm::translate(glm::vec3(-4.7f, 0.74f, -6.8f));
    model = translation * rotation * scale;
    gSoccerBall.model = model;
    
    // Floor
    scale = glm::scale(glm::vec3(10.0f, 10.0f, 10.0f));
    translation = glm::translate(glm::vec3(10.0f, 0.0f, 0.0f));
    model = translation * rotation * scale;
    gFloor.model = model;

    // Trim
    translation = glm::translate(glm::vec3(-10.0f, 0.01f, -9.9999f));
    model = translation * rotation * scale;
    for (unsigned int i = 0; i < gTrim.size(); i++) {
        gTrim.at(i).model = model;
    }
    // Bottom of wall
    rotation = glm::rotate(PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
    scale = glm::scale(glm::vec3(10.0f, 10.0f, 10.0f));
    translation = glm::translate(glm::vec3(10.0f, 10.0f, -10.0f));
    model = translation * rotation * scale;
    gWallBottom.model = model;

    // Top of wall
    rotation = glm::rotate(PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
    scale = glm::scale(glm::vec3(10.0f, 10.0f, 10.0f));
    translation = glm::translate(glm::vec3(10.0f, 14.0f, -10.0f));
    model = translation * rotation * scale;
    gWallTop.model = model;

    // Coffee table
    scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
    rotation = glm::rotate(1.5701f, glm::vec3(0.0f, 1.0f, 0.0f));
    translation = glm::translate(glm::vec3(0.0f, 2.661f, 0.0f));
    model = translation * rotation * scale;
    for (unsigned int i = 0; i < gCoffeeTable.size(); i++) {
        gCoffeeTable.at(i).model = model;
    }
    // Couch
    scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
    for (unsigned int i = 0; i < gCouch.size(); i++) {
        if (i < 3) {
            // Draw bottom cushion
            translation = glm::translate(glm::vec3(-3.0f, 2.5f, -7.2f));
            rotation = glm::rotate(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            model = translation * rotation * scale;
            gCouch.at(i).model = model;
        }
        else if (i > 2 && i < 6) {
            // Draw back cushion
            translation = glm::translate(glm::vec3(-3.0f, 4.5f, -8.0f));
            rotation = glm::rotate(1.309f, glm::vec3(1.0f, 0.0f, 0.0f));
            model = translation * rotation * scale;
            gCouch.at(i).model = model;
        }
        else if (i > (gCouch.size() - 3)) {
            translation = glm::translate(glm::vec3(-3.3f, 1.85f, -8.199f));
            rotation = glm::rotate((float)(PI/2), glm::vec3(1.0f, 0.0f, 0.0f));
            model = translation * rotation * scale;
            gCouch.at(i).model = model;
        }
        else {
            translation = glm::translate(glm::vec3(-3.0f, 1.5f, -7.2f));
            rotation = glm::rotate(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            model = translation * rotation * scale;
            gCouch.at(i).model = model;
        }
    }

    // Position lights
    rotation = glm::rotate(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    translation = glm::translate(gLight1Position);
    model = translation * rotation * scale;
    gLight1.model = model;

    translation = glm::translate(gLight2Position);
    model = translation * rotation * scale;
    gLight2.model = model;

    // Position the lamp
    rotation = glm::rotate(PI, glm::vec3(1.0f, 0.0f, 0.0f));
    scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
    translation = glm::translate(glm::vec3(-5.1f, 4.3f, -8.3f));
    
    for (unsigned int i = 0; i < gLamp.size(); i++) {
        model = translation * rotation * scale;
        rotation = glm::rotate(PI, glm::vec3(1.0f, 0.0f, -1.0f));
        gLamp.at(i).model = model;
    }
}

// Create vertex array objects for meshes
void CreateVAOS(GLMesh& mesh) {
    // Set sizes for data being passed to OpenGL
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    // Set the number of indices
    mesh.nIndices = mesh.indices.size();

    // Generate vertex arrays
    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffer, one for the data and one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(GLfloat), mesh.vertices.data(), GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(GLushort), mesh.indices.data(), GL_STATIC_DRAW);  // Transfer data to GPU

    // Strides between vertex coordinates is 8 (x, y, z, nx, ny, nz, u, v). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Create a plane for wall or floor
void CreatePlane(GLMesh& mesh, Vertex frontRight, GLfloat length, GLfloat width, GLuint texture) {
    vec3 normal(0.0f, 1.0f, 0.0f);

    // Create vertices for corners of plane
    mesh.vertices = { frontRight.x, frontRight.y, frontRight.z, normal.x, normal.y, normal.z, 1.0f, 1.0f,
            frontRight.x, frontRight.y, frontRight.z - length, normal.x, normal.y, normal.z, 1.0f, 0.0f,
            frontRight.x - width, frontRight.y, frontRight.z - length, normal.x, normal.y, normal.z, 0.0f, 0.0f,
            frontRight.x - width, frontRight.y, frontRight.z, normal.x, normal.y, normal.z, 0.0f, 1.0f
    };
    // Indices for drawing the triangles
    mesh.indices = { 0, 1, 3,  // Triangle 1
                     1, 2, 3   // Triangle 2
    };
    // Create VAOS/VBOS and assign texture
    CreateVAOS(mesh);
}

// Create lamp
void CreateLamp(vector<GLMesh>& meshArray) {
    GLMesh mesh;
    Sphere base(0.4f, 12, 32, 0.0f, 0.0f, 0.0f);
    mesh.vertices = base.GetVertices();
    mesh.vertices.resize(mesh.vertices.size() / 2);
    mesh.indices = base.GetIndices();
    mesh.indices.resize(mesh.indices.size() / 2);
    mesh.texture = gLampTexture;
    CreateVAOS(mesh);
    meshArray.push_back(mesh);

    Cylinder stand(1.5f, 0.1f, 0.0f, -0.3f, 0.0f, BOTTOM);
    mesh.vertices = stand.GetVertices();
    mesh.indices = stand.GetIndices();
    mesh.texture = gLampTexture;
    CreateVAOS(mesh);
    meshArray.push_back(mesh);

    Cylinder shade(1.1f, 0.8f, 0.0f, -1.7f, 0.0f, NONE);
    mesh.vertices = shade.GetVertices();
    mesh.indices = shade.GetIndices();
    mesh.texture = gLampShadeTexture;
    CreateVAOS(mesh);
    meshArray.push_back(mesh);
}

// Create the end table
void CreateEndTable(vector<GLMesh>& meshArray) {
    
    GLMesh mesh;

    // Create cuboids for flat surfaces
    Cuboid topSurface(0.8f, 1.0f, 0.2f, -0.5f, 1.0f, -1.0f, 1);
    mesh.vertices = topSurface.GetVertices();
    mesh.indices = topSurface.GetIndices();
    mesh.texture = gEndTableSurfacesTexture;
    CreateVAOS(mesh);
    meshArray.push_back(mesh);

    Cuboid bottomSurface(2.0f, 1.0f, 0.2f, -0.5f, 0.0f, -1.0f, 1);
    mesh.vertices = bottomSurface.GetVertices();
    mesh.indices = bottomSurface.GetIndices();
    mesh.texture = gEndTableSurfacesTexture;
    CreateVAOS(mesh);
    meshArray.push_back(mesh);

    // Create cylinders for end table supports and legs
    Cylinder backLeftSupport(1.0f, 0.1f, -0.4f, 0.8f, -0.9f, NONE);
    Cylinder middleLeftSupport(1.0f, 0.1f, -0.4f, 0.8f, -0.6f, NONE);
    Cylinder frontLeftSupport(1.0f, 0.1f, -0.4f, 0.8f, -0.3f, NONE);
    Cylinder backRightSupport(1.0f, 0.1f, 0.4f, 0.8f, -0.9f, NONE);
    Cylinder middleRightSupport(1.0f, 0.1f, 0.4f, 0.8f, -0.6f, NONE);
    Cylinder frontRightSupport(1.0f, 0.1f, 0.4f, 0.8f, -0.3f, NONE);
    Cylinder backLeftLeg(1.0f, 0.1f, -0.4f, -0.2f, -0.9f, BOTTOM);
    Cylinder backRightLeg(1.0f, 0.1f, 0.4f, -0.2f, -0.9f, BOTTOM);
    Cylinder frontLeftLeg(1.0f, 0.1f, -0.4f, -0.2f, 0.9f, BOTTOM);
    Cylinder frontRightLeg(1.0f, 0.1f, 0.4f, -0.2f, 0.9f, BOTTOM);
    
    // Add cylinders to vector to use in loop
    vector<Cylinder> legsAndSupports = { backLeftSupport, middleLeftSupport, frontLeftSupport, backRightSupport, middleRightSupport, frontRightSupport, backLeftLeg, backRightLeg, frontLeftLeg, frontRightLeg };

    // Generate vertices and indices, store in local variable, then add to vector of meshes
    for (unsigned int i = 0; i < legsAndSupports.size(); i++) {
        mesh.vertices = legsAndSupports.at(i).GetVertices();
        mesh.indices = legsAndSupports.at(i).GetIndices();
        mesh.texture = gEndTableCylindersTexture;
        meshArray.push_back(mesh);
    }

    // Create the VAOs for each part of the end table
    for (unsigned int i = 0; i < meshArray.size(); i++) {
        CreateVAOS(meshArray.at(i));
    }
}

// Create the coffee table
void CreateCoffeeTable(vector<GLMesh>& meshArray) {
    GLMesh tempMesh;

    Cuboid topSurface(4.0f, 3.0f, 0.2f, -2.5f, 0.3f, -1.0f, 1);
    Cuboid frontLeftLeg(0.2f, 0.2f, 1.53f, -2.3f, 0.2f, -0.9f, 1);
    Cuboid backLeftLeg(0.2f, 0.2f, 1.53f, 0.1f, 0.2f, -0.9f, 1);
    Cuboid frontRightLeg(0.2f, 0.2f, 1.53f, -2.3f, 0.2f, 2.7f, 1);
    Cuboid backRighttLeg(0.2f, 0.2f, 1.53f, 0.1f, 0.2f, 2.7f, 1);
    Cuboid frontUnderpinning(3.5f, 0.1f, 0.3f, -2.25f, 0.2f, -0.8f, 1);
    Cuboid backUnderpinning(3.5f, 0.1f, 0.3f, 0.15f, 0.2f, -0.8f, 1);
    Cuboid leftUnderpinning(0.1f, 2.4f, 0.3f, -2.2f, 0.2f, -0.85f, 1);
    Cuboid rightUnderpinning(0.1f, 2.4f, 0.3f, -2.2f, 0.2f, 2.75f, 1);

    // Place pieces into vector to run through loop
    vector<Cuboid> coffeeTable = { topSurface, frontLeftLeg, backLeftLeg, frontRightLeg, backRighttLeg, frontUnderpinning, backUnderpinning, leftUnderpinning, rightUnderpinning };
    for (unsigned int i = 0; i < coffeeTable.size(); i++) {
        // Vector items 5 and 6 are the front and back underpinnings, adjust texture accordingly
        if (i == 5 || i == 6) {
            coffeeTable.at(i).SetTiles(0.25f, 1.0f, 1.0f);
        }
        // Vector items 7 and 8 are the left and right underpinnings, adjust texture accordingly
        else if (i == 7 || i == 8) {
            coffeeTable.at(i).SetTiles(1.0f, 0.25f, 1.0f);
        }
        // Generate the vertices and indices in the cuboid object, then retrieve them and place in this piece's vertices and indices vectors
        coffeeTable.at(i).GenCuboid();
        tempMesh.vertices = coffeeTable.at(i).GetVertices();
        tempMesh.indices = coffeeTable.at(i).GetIndices();
        // The first five pieces use the same texture
        if (i < 5) {
            tempMesh.texture = gCoffeeTableTopTexture;
        }
        // As do the last 4
        else {
            tempMesh.texture = gCoffeeTableUnder;
        }
        // Add to the mesh
        meshArray.push_back(tempMesh);
    }
    // Create the VAOs for each part of the coffee table
    for (unsigned int i = 0; i < meshArray.size(); i++) {
        CreateVAOS(meshArray.at(i));
    }
}

// Create the couch
void CreateCouch(vector<GLMesh>& meshArray) {
    GLMesh tempMesh;

    Cuboid leftCushion(3.0f, 2.98f, 1.0f, 0.0f, 0.0f, 0.0f, 1);
    Cuboid centerCushion(3.0f, 2.98f, 1.0f, 3.0f, 0.0f, 0.0f, 1);
    Cuboid rightCushion(3.0f, 2.98f, 1.0f, 6.0f, 0.0f, 0.0f, 1);
    Cuboid backLeftCushion(3.0f, 2.98f, 1.0f, 0.0f, 0.0f, 0.0f, 1);
    Cuboid backCenterCushion(3.0f, 2.98f, 1.0f, 3.0f, 0.0f, 0.0f, 1);
    Cuboid backRightCushion(3.0f, 2.98f, 1.0f, 6.0f, 0.0f, 0.0f, 1);
    Cuboid base(5.0f, 10.0f, 1.47f, -0.5f, -0.02f, -2.0f, 1);
    Cuboid leftArm(4.99f, 0.497f, 1.54f, -0.499f, 1.5f, -2.0f, 1);
    Cuboid rightArm(4.99f, 0.497f, 1.54f, 9.001f, 1.5f, -2.0f, 1);
    Cuboid back(0.51f, 10.01f, 3.79f, -0.5001f, 2.3f, -1.999f, 1);
    Cylinder leftArmCap(5.02f, 0.4f, 0.0f, 4.0f, -1.0f, BOTH);
    Cylinder rightArmCap(5.02f, 0.402f, 9.6f, 4.0f, -1.0f, BOTH);

    vector<Cuboid> couch = { leftCushion, centerCushion, rightCushion, backLeftCushion, backCenterCushion, backRightCushion, base, leftArm, rightArm, back};

    for (unsigned int i = 0; i < couch.size(); i++) {
        // Generate the vertices and indices in the cuboid object, then retrieve them and place in this piece's vertices and indices vectors
        tempMesh.vertices = couch.at(i).GetVertices();
        tempMesh.indices = couch.at(i).GetIndices();
        // The first six pieces use the same texture
        if (i < 6) {
            tempMesh.texture = gCouchCushionTexture;
        }
        else {
            tempMesh.texture = gCouchTexture;
        }
        // Add to the mesh
        meshArray.push_back(tempMesh);
    }

    tempMesh.vertices = leftArmCap.GetVertices();
    tempMesh.indices = leftArmCap.GetIndices();
    tempMesh.texture = gCouchTexture;
    meshArray.push_back(tempMesh);
    
    tempMesh.vertices = rightArmCap.GetVertices();
    tempMesh.indices = rightArmCap.GetIndices();
    tempMesh.texture = gCouchTexture;
    meshArray.push_back(tempMesh);


    // Create the VAOs for each part of the coffee table
    for (unsigned int i = 0; i < meshArray.size(); i++) {
        CreateVAOS(meshArray.at(i));
    }
}

// Function to change the size of a GLFWwindow
void ChangeSize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Function to process user input
void ProcessInput(GLFWwindow* window) {
    // Set camera speed
    static const float cameraSpeed = 2.5f;

    // Amount camera has moved
    float cameraOffset = cameraSpeed * deltaTime;

    // Check if user pressed a key and convert into moving the camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }

    // Enable shutdown proceedure if user presses the Escape key
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Destroy a mesh
void DestroyMesh(GLMesh& mesh) {
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, &mesh.vbos[0]);
    glDeleteBuffers(1, &mesh.vbos[1]);
}

// Destroy the shader program
void DestroyShaderProgram(GLuint programId) {
    glDeleteProgram(programId);
}

// Test resources. Input is a compiled vertex or fragment shader or a linked program. Resource is the type and can be VERTEX, FRAGMENT, or PROGRAM
bool TestResource(GLuint input, Resource resource) {
    int success;                    // Used to store success or failure
    char errorInfo[512];            // Used to store error information

    // Get status of compile/link operation
    switch (resource) {
    case(VERTEX):
    case(FRAGMENT):
        glGetShaderiv(input, GL_COMPILE_STATUS, &success);
        break;
    case(PROGRAM):
        glGetProgramiv(input, GL_LINK_STATUS, &success);
        break;
    default:
        cout << "LOGIC ERROR!" << endl;
        return false;
    }

    // If compile/link was unsuccessful, display error
    if (!success) {
        switch (resource) {
            // Shaders
        case(VERTEX):
        case(FRAGMENT):
            // Get error information
            glGetShaderInfoLog(input, 512, NULL, errorInfo);
            break;
            // Program
        case(PROGRAM):
            // Get error information
            glGetProgramInfoLog(input, 512, NULL, errorInfo);
            break;
        default:
            cout << "LOGIC ERROR" << endl;
            return false;
        }
        cout << "Error! ";

        // Output text based on resource type
        switch (resource) {
        case(VERTEX):
            cout << "Vertex ";
            break;
        case(FRAGMENT):
            cout << "Fragment ";
            break;
        case(PROGRAM):
            cout << "Program ";
            break;
        default:
            cout << "ERROR! ";
            break;
        }
        cout << "Resource usage failed!\n" << errorInfo << endl;
        return false;
    }
    return true;
}

// This function enables reading of mouse movement
void MousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
    // If the mouse just entered focus for the first time
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        // Ensure this doesn't repeat
        firstMouse = false;
    }

    // Determine how far the mouse has moved
    float xOffset = (float)(xpos - lastX);
    float yOffset = (float)(lastY - ypos);

    // Store current mouse position
    lastX = (float)xpos;
    lastY = (float)ypos;

    // Send mouse movement to camera controls
    camera.ProcessMouseMovement(xOffset, yOffset);
}

// This function enables reading of mouse scroll wheel movement
void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // Send the scroll wheel movement to the camera controls
    camera.ProcessMouseScroll((float)yoffset);
}

// This function catches a press of the p key for perspective and ortho projection toggling and changing light colors
// This callback was used because the normal key callback would repeat the keypress even with a short press of the key
void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        perspective = !perspective;
    }

    // Modify light 1's color when F1 is pressed
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        static bool Light1Colored = false;
        Light1Colored = !Light1Colored;
        if (Light1Colored) {
            gLight1Color = glm::vec3(0.754f, 0.471f, 0.104f);
        }
        else {
            gLight1Color = glm::vec3(1.0f, 1.0f, 0.941f);
        }
    }

    // Modify light 2's color when F2 is pressed
    if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
        static bool Light2Colored = false;
        Light2Colored = !Light2Colored;
        if (Light2Colored) {
            gLight2Color = glm::vec3(0.254f, 0.471f, 0.104f);
        }
        else {
            gLight2Color = glm::vec3(1.0f, 1.0f, 0.778f);
        }
    }
}

// Load textures
void LoadTexture(GLuint& texture, string filename, GLuint textureNum) {
    // Create texture id
    glGenTextures(1, &texture);
    // Activate the texture
    glActiveTexture(GL_TEXTURE0 + textureNum);
    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Variables for storing image dimensions
    int width, height, nrChannels;
    // Load image upside down
    stbi_set_flip_vertically_on_load(true);
    // Load the image from the file
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    // Test if image was loaded successfully
    if (data) {
        // Generate the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // Generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        // Notify user of error
        cout << "Failed to load texture" << endl;
    }

    // Release resources
    stbi_image_free(data);
}

void DestroyTextures() {
    glGenTextures(1, &gFloor.texture);
    glGenTextures(1, &gWallBottom.texture);
    glGenTextures(1, &gWallTop.texture);
    glGenTextures(1, &gEndTableSurfacesTexture);
    glGenTextures(1, &gEndTableCylindersTexture);
    glGenTextures(1, &gCoffeeTableTopTexture);
    glGenTextures(1, &gCoffeeTableUnder);
    glGenTextures(1, &gSoccerBall.texture);
    glGenTextures(1, &gTrimTexture);
    glGenTextures(1, &gCouchCushionTexture);
    glGenTextures(1, &gLampTexture);
    glGenTextures(1, &gLampShadeTexture);
}

// Create a shader program. Requires source for vertex and fragment shaders and the program id to update
bool CreateShaderProgram(const char* VertexShaderSource, const char* FragmentShaderSource, GLuint& programId) {

    // Create the shader program object
    programId = glCreateProgram();

    // Create fragment and vertex shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Assign the shader sources to each shader
    glShaderSource(vertexShader, 1, &VertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, &FragmentShaderSource, NULL);

    // Compile shaders and test for errors
    glCompileShader(vertexShader);
    if (!TestResource(vertexShader, VERTEX))
        return false;
    glCompileShader(fragmentShader);
    if (!TestResource(fragmentShader, FRAGMENT))
        return false;

    // Attach compiled shaders to program
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    glLinkProgram(programId);
    if (!TestResource(programId, PROGRAM))
        return false;

    return true;
}