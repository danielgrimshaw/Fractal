
// OpenGL
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/glut.h>

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standard Libraries
#include <iostream>

// Header Files
#include "util.h"
#include "Shader.h"
#include "Program.h"

// Handlers
void draw_handler(void); // Redraw handler
void idle_handler(void); // Idle handler
void resize_handler(int width, int height); // Resize Handler
void key_handler(unsigned char key, int x, int y); // Keyboard driver
void button_handler(int bn, int state, int x, int y); // Mouse button handler
void mouse_handler(int x, int y); // Mouse motion handler
void mouse_idle_handler(int x, int y); // Passive mouse motion handler

// Filenames
std::string mbrot_vertex_name = "mbrot_vert.glsl";
std::string mbrot_fragment_name = "mbrot_frag.glsl";
std::string julia_vertex_name = "julia_vert.glsl";
std::string julia_fragment_name = "julia_frag.glsl";

// Shaders
Shader mbrot_vertex = Shader(GL_VERTEX_SHADER, mbrot_vertex_name);
Shader mbrot_fragment = Shader(GL_FRAGMENT_SHADER, mbrot_fragment_name);
Shader julia_vertex = Shader(GL_VERTEX_SHADER, julia_vertex_name);
Shader julia_fragment = Shader(GL_FRAGMENT_SHADER, julia_fragment_name);

// Programs
Program mandelbrot = Program(mbrot_vertex, mbrot_fragment);
Program julia = Program(julia_vertex, julia_fragment);

// Buffer Objects
GLuint VBO, VAO, EBO;

// Camera
glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f); // Location of Camera
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f); // Where the camera is pointing
glm::vec3 camera_dir = glm::normalize(camera_pos - camera_target); // Direction from the origin to the camera
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camera_right = glm::normalize(glm::cross(up, camera_dir));
glm::vec3 camera_up = glm::cross(camera_dir, camera_right);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);

// Uniforms
glm::mat4 model_transform; // Transformation for each model on the scene
glm::mat4 view; // Transformation of the scene for the camera
glm::mat4 projection; // Projection matrix (simulates perspective)

GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;
GLfloat fov = 45.0f;

// Controls list
const char * controls = "Controls:\r\n"
"\'c\', \'h\', or \'?\': Print these controls\r\n"
"\'m\': Go to mandelbrot mode\r\n"
"\'j\': Go to jula mode\r\n"
"\'+\': Increase iterations\r\n"
"\'-\': Decrease iterations\r\n"
"\r\n"
"Mandelbrot additional controls:\r\n"
"\tClick and drag: Zoom in and out\r\n"
"\tMouse scroll: Zoom in and out\r\n"
"\r\n"
"Julia additional controls:\r\n"
"\tMouse click or drag: Change C value (shape)\r\n"
"\tSpace bar: Toggle interactive mode\r\n";

int main(int argc, char ** argv) {
	using namespace std;
	
	GLfloat vertices[] = {
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f
	};

	GLuint indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	// Print controls
	cout << controls << endl;

	// initialize glut
	
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Fractals");

	glutDisplayFunc(draw_handler);
	glutIdleFunc(idle_handler);
	glutReshapeFunc(resize_handler);
	glutKeyboardFunc(key_handler);
	glutMouseFunc(button_handler);
	glutMotionFunc(mouse_handler);
	glutPassiveMotionFunc(mouse_idle_handler);
	
	// Load all library function pointers
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { // Doesn't run without GLEW
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    } 

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// Create Shaders
	mbrot_vertex.create();
	mbrot_fragment.create();
	julia_vertex.create();
	julia_fragment.create();

	// Create Programs
	mandelbrot.create();
	julia.create();

	// Set up camera
	projection = glm::perspective(glm::radians(fov),
		800.0f / 600.0f, 0.1f, 100.0f);
	view = glm::lookAt(camera_pos, camera_target, up);
	
	glUseProgram(mandelbrot.getId());
	cout << "Set to mandelbrot" << endl;
	
	glViewport(0,0,800,600); // Tell GPU where to draw to and window size
	glutMainLoop(); // Enter callback loop
	return 0;
}

void draw_handler(void) {
	GLint uniform_loc;
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(mandelbrot.getId());

	uniform_loc = glGetUniformLocation(mandelbrot.getId(), "model_transform");
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(model_transform));

	uniform_loc = glGetUniformLocation(mandelbrot.getId(), "view");
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(view));

	uniform_loc = glGetUniformLocation(mandelbrot.getId(), "projection");
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glutSwapBuffers();
}

void idle_handler(void) {
	glutPostRedisplay();
}

void resize_handler(int width, int height) {
	// pass
	glutPostRedisplay();
}

void key_handler(unsigned char key, int x, int y) {
	using namespace std;
	GLfloat speed = 0.1f;

	cout << "detected " << key << endl;
	switch (key) { // Keyboard
	case 27: // ESC
	case 'q':
	case 'Q': // Quit
		exit(0);
		break;
	case 'w':
	case 'W':
		camera_pos += glm::normalize(glm::cross(camera_front, camera_right))
			* speed;
		cout << "responding to " << key << endl;
		break;
	case 's':
	case 'S':
		camera_pos -= glm::normalize(glm::cross(camera_front, camera_right))
			* speed;
		cout << "responding to " << key << endl;
		break;
	case 'a':
	case 'A':
		camera_pos += glm::normalize(glm::cross(camera_front, camera_up))
			* speed;
		cout << "responding to " << key << endl;
		break;
	case 'd':
	case 'D':
		camera_pos -= glm::normalize(glm::cross(camera_front, camera_up))
			* speed;
		cout << "responding to " << key << endl;
		break;
	default: // No key was pressed that is of importance
		break;
	}

	view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);

	glutPostRedisplay();
}

void button_handler(int bn, int state, int x, int y) {
	switch (bn) {
	case 3: // Scroll up
	//	fov += 1.0f;
		break;
	case 4: // Scroll down
	//	fov -= 1.0f;
		break;
	}

	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 179.0f)
		fov = 179.0f;

	projection = glm::perspective(glm::radians(fov),
		800.0f / 600.0f, 0.1f, 100.0f);

	glutPostRedisplay();
}

void mouse_handler(int x, int y) {
	//pass
	glutPostRedisplay();
}

void mouse_idle_handler(int x, int y) {
	//pass
	glutPostRedisplay();
}