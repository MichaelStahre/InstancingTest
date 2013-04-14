#include <stdio.h>
#include <string>

#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include <GL\glew.h>
#include <GL\glfw.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>


// Uses
using std::string;


// Constants
const int MAX_ENTITY_COUNT = 2000;

// Types
struct entity {
  glm::vec3 position;
  glm::vec2 size;
};

struct camera {
  glm::vec3 position;
  float zoom;
};


// Members
bool keep_running = true;

camera player_camera;
entity entities[MAX_ENTITY_COUNT];

GLuint shaderProgram, vbo, vertexShader, fragmentShader;
GLint pLoc, insPosLoc, insSizeLoc;
GLuint instance_buffer;


// Forward declarations, just trying to keep this test tiny in its file count.

int CreateWindow();
char* readFile(const string& filename);

void init();
void update(const double& dt);
void render(const double& dt);


/*

Creates an OpenGL window with the newest available version on your system.
For me, that's a 4.3 context. This application uses features only available in 3.3 and later.

*/
int CreateWindow() {

  int width(1024), height(768), bitdepth(24),
    alphabits(0), depthbits(0), stencilbits(0),
    mode(GLFW_WINDOW);

  return glfwOpenWindow(width, height, bitdepth/3, bitdepth/3, bitdepth/3, alphabits, depthbits, stencilbits, mode);
}



// Initializes GLFW, GLEW and this application and then gets a rudimentary game loop going.

int main() {

  // Initialize GLFW
  switch(glfwInit()) {
  case GL_TRUE:
    fprintf(stdout, "GLFW Init complete \n");
    break;
  case GL_FALSE:
    fprintf(stderr, "glfwInit did not succeed \n");
    exit(0);
    break;
  }

  int glfw_major, glfw_minor, glfw_rev;
  glfwGetVersion(&glfw_major, &glfw_minor, &glfw_rev);
  fprintf(stdout, "Using GLFW %d.%d.%d \n", glfw_major, glfw_minor, glfw_rev);

  switch(CreateWindow()) {
  case GL_TRUE:
    fprintf(stdout, "CreateWindow complete \n");
    break;
  case GL_FALSE:
    fprintf(stderr, "CreateWindow did not succeed \n");
    exit(0);
    break;
  }

  switch(glewInit()) {
  case GLEW_OK:
    fprintf(stdout, "GLEW Init complete \n");
    break;
  default:
    fprintf(stderr, "glewInit did not succeed \n");
    glfwTerminate();
    exit(0);
  }

  glfwSwapInterval(1);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

  fprintf(stdout, "GLFW and GLEW configuration complete.\n");


  init();

  double last_tick = glfwGetTime();
  while(keep_running) {
    update(glfwGetTime()-last_tick);
    render(glfwGetTime()-last_tick);
    last_tick = glfwGetTime();

    glfwSwapBuffers();

  }

  glfwTerminate();

  return 0;

}



/* 
Reads the specified file and return a pointer to a char array containing it.

Remember to free the char* memory when you're done with it!
*/
char* readFile(const string& filename) {
  string out;
  std::ifstream file;
  file.open(filename.c_str());
  if (file) {
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    out = stream.str();
  } else {
    out = "File \"" + filename + "\" not found.";
  }

  char* ret = new char[out.size()];
  strcpy(ret, out.c_str());
  return ret;
}


/*
  Initializes everything that's necessary for this test in the following order:

  * Reads and compiles the shaders
  * Sets up the Vertex Buffers and the VertexAttribPointers into them, as well as obtaining the one Uniform location required
  * Sets up the camera
  * Creates 2000 entities, with positions that should leave them fairly spread out across the screen
*/
void init() {

  // Read the shaders from their respective files
  const char *vtexshader(readFile("vertex_shader.glsl")),
    *fragshader(readFile("fragment_shader.glsl"));


  // Compile the vertex shader
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vtexshader, NULL);
  glCompileShader(vertexShader);
  GLint status;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
  switch(status) {
  case GL_TRUE:
    fprintf(stdout, "Vertex Shader successfully compiled.\n");
    break;
  default:
    char buffer[513];
    glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
    fprintf(stderr, "Vertex Shader failed to compile. Log: \n");
    fprintf(stderr, buffer);
    break;
  }

  // Compile the fragment shader
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragshader, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
  switch(status) {
  case GL_TRUE:
    fprintf(stdout, "Fragment Shader successfully compiled.\n");
    break;
  default:
    char buffer[513];
    glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
    fprintf(stderr, "Fragment Shader failed to compile. Log: \n");
    fprintf(stderr, buffer);
    break;
  }

  // Set up the shaderprogram
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);



  // Shaders done, time to set up the application's two vertex buffers


  // This is the square I draw the sprites on
  float vertices[] = { // X, Y, texcoord x, texcoord y
    0.0f, 0.0f, 0.0f, 0.0f, //top left
    1.0f, 0.0f, 1.0f, 0.0f, //top right
    0.0f, 1.0f, 0.0f, 1.0f, // bottom left
    0.0f, 1.0f, 0.0f, 1.0f, // bottom left
    1.0f, 0.0f, 1.0f, 0.0f, //top right
    1.0f, 1.0f, 1.0f, 1.0f // bottom right
  };

  glGenBuffers(1, &vbo); // The buffer that is bound when I call DrawArraysInstanced()
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

  // Set up the VertexAttribPointers for the vertex shader
  GLint posAttrib = glGetAttribLocation(shaderProgram, "model_position");
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(posAttrib);

  GLint texAttrib = glGetAttribLocation(shaderProgram, "texCoord");
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
  glEnableVertexAttribArray(texAttrib);

  // Set up the instance data buffer and its VertexAttribPointers
  glGenBuffers(1, &instance_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);

  insPosLoc = glGetAttribLocation(shaderProgram, "instance_position");
  glVertexAttribPointer(insPosLoc, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)0);
  glVertexAttribDivisor(insPosLoc, 1);

  insSizeLoc = glGetAttribLocation(shaderProgram, "instance_size");
  glVertexAttribPointer(insSizeLoc, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
  glVertexAttribDivisor(insSizeLoc, 1);

  // Get the uniform location for the vertex shader's projection matrix
  pLoc = glGetUniformLocation(shaderProgram, "proj");


  // Set up the camera
  player_camera.position = glm::vec3(0.0f);
  player_camera.zoom = 800.0f;

  // Set up a bunch of entitys to draw
  for(size_t i = 0; i < MAX_ENTITY_COUNT; ++i) {
    entities[i].position = glm::vec3(100.0f + 100.0f*cos(i*100.0f), 100.0f + 100.0f*sin(i*100.0f), 0.0f);
    entities[i].size = glm::vec2(64.0f, 64.0f);
  }


}


/*
  Reacts to the little input this test cares about.

  Esc turns off the application.
  T zooms in, G zooms out, B resets zoom.
  Arrow keys move the camera around.
*/
void update(const double& dt) {

  // Key for turning the game off
  if(glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS) {
    keep_running = false;
  }


  // Keys for manipulating and resetting camera zoom
  if(glfwGetKey('T') == GLFW_PRESS)
    player_camera.zoom -= player_camera.zoom*(float)dt;

  if(glfwGetKey('G') == GLFW_PRESS)
    player_camera.zoom += player_camera.zoom*(float)dt;

  if(glfwGetKey('B') == GLFW_PRESS)
    player_camera.zoom = 800.0f;

  // Keys for manipulating camera position
  if(glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS)
    player_camera.position.x -= player_camera.zoom*(float)dt;

  if(glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS)
    player_camera.position.x += player_camera.zoom*(float)dt;

  if(glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS)
    player_camera.position.y -= player_camera.zoom*(float)dt;

  if(glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS)
    player_camera.position.y += player_camera.zoom*(float)dt;

}


/*
  Calculates the projection matrix, feeds it into its uniform, puts in all the current entity data in the instance buffer and renders the whole thing.
*/
void render(const double& dt) {
  glClear(GL_COLOR_BUFFER_BIT);

  // Calculate the current projection matrix
  float aspect = 1024.0f/768.0f;      
  glm::mat4 proj = glm::ortho(player_camera.position.x,
    player_camera.position.x + player_camera.zoom,
    player_camera.position.y + player_camera.zoom/aspect,
    player_camera.position.y, -1.0f, 0.0f);

  // Update the uniform to the new projection matrix
  glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(proj));

  // Put all the current entity data into the instance data buffer
  glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(entity)*MAX_ENTITY_COUNT, entities, GL_STREAM_DRAW);

  // Now bind the vertex buffer with the square in it and draw away.
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, MAX_ENTITY_COUNT);

}