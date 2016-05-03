/*
* Fractals.cpp
*
* Copyright 2016  <Daniel Grimshaw>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
* MA 02110-1301, USA.
*
*
*/
// OpenGL
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/glut.h>

// Standard Libraries
#include <iostream>

// Header Files
#include "util.h"
#include "Shader.h"
#include "Program.h"

// Functions
void updateAspect(void);
void updateDeltas(void);

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

// FPS Counter
double lastTime = 0;
int nbFrames = 0;

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

// Uniforms
float min_X = -1.75;
float max_X = 1.75;
float min_Y = -1.5;
float max_Y = 1.5;
float delta_X;
float delta_Y;

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
	
	glUseProgram(mandelbrot.getId());
	cout << "Set to mandelbrot" << endl;
	
	glViewport(0,0,800,600); // Tell GPU where to draw to and window size

	updateAspect();
	lastTime = glutGet(GLUT_ELAPSED_TIME);

	glutMainLoop(); // Enter callback loop
	return 0;
}

void updateAspect(void) {
	float aspect = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	if ((max_X - min_X) / (max_Y - min_Y) != aspect) {
		min_X *= aspect;
		max_X *= aspect;
	}
}

void updateDeltas(void) {
	delta_X = (max_X - min_X);// / 2.0f;
	delta_Y = (max_Y - min_Y);// / 2.0f;
}

void draw_handler(void) {
	using namespace std;
	GLint uniform_loc;

	//updateAspect();
	updateDeltas();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(mandelbrot.getId());

	uniform_loc = glGetUniformLocation(mandelbrot.getId(), "min_X");
	glUniform1f(uniform_loc, min_X);

	uniform_loc = glGetUniformLocation(mandelbrot.getId(), "max_X");
	glUniform1f(uniform_loc, max_X);

	uniform_loc = glGetUniformLocation(mandelbrot.getId(), "min_Y");
	glUniform1f(uniform_loc, min_Y);

	uniform_loc = glGetUniformLocation(mandelbrot.getId(), "max_Y");
	glUniform1f(uniform_loc, max_Y);

	uniform_loc = glGetUniformLocation(mandelbrot.getId(), "delta_X");
	glUniform1f(uniform_loc, delta_X);
	
	uniform_loc = glGetUniformLocation(mandelbrot.getId(), "delta_Y");
	glUniform1f(uniform_loc, delta_Y);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glutSwapBuffers();
	
	double currentTime = glutGet(GLUT_ELAPSED_TIME);
	double deltaT = currentTime - lastTime;
	nbFrames++;
	if (deltaT >= 1000){
		cout << 1000.0/(double)nbFrames << " ms/frame (" << nbFrames << " fps)" << endl;
		nbFrames = 0;
		lastTime = currentTime;
	}
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

	switch (key) { // Keyboard
	case 27: // ESC
	case 'q':
	case 'Q': // Quit
		exit(0);
		break;
	case 'w':
	case 'W':
		min_Y += speed;
		max_Y += speed;
		break;
	case 's':
	case 'S':
		min_Y -= speed;
		max_Y -= speed;
		break;
	case 'a':
	case 'A':
		min_X -= speed;
		max_X -= speed;
		break;
	case 'd':
	case 'D':
		min_X += speed;
		max_X += speed;
		break;
	default: // No key was pressed that is of importance
		break;
	}

	glutPostRedisplay();
}

void button_handler(int bn, int state, int x, int y) {
	GLfloat inSpeed = 0.99f;
	GLfloat outSpeed = 1.01f;
	switch (bn) {
	case 3: // Scroll up
		min_X *= inSpeed;
		max_X *= inSpeed;
		min_Y *= inSpeed;
		max_Y *= inSpeed;
		break;
	case 4: // Scroll down
		min_X *= outSpeed;
		max_X *= outSpeed;
		min_Y *= outSpeed;
		max_Y *= outSpeed;
		break;
	}

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
