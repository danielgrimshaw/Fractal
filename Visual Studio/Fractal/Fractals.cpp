#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <cstdlib>
#include "util.h"
#include "Fractals.h"

float cx = 0.7f, cy = 0.0f;
float scale = 2.2f;
int iter = 70;
const float zoom_factor = 0.025f;
static Shader * shaders = new Shader;
static Texture * textures = new Texture;
static Camera * camera = new Camera(shaders, -0.5f, 0.0f, 2.5f, 0.0f, 0.0f);

GLfloat vertices[12] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
};

GLuint indices[6] = {
	0, 1, 2,
	1, 2, 3
};

GLuint VAO, VBO, EBO;
void startFractal() {
	int argc = 0;
	char ** argv = NULL;

	// initialize glut (window manager)
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Mandelbrot");

	glViewport(0, 0, 800, 600);

	glutDisplayFunc(draw);
	glutIdleFunc(idle_handler);
	glutKeyboardFunc(key_handler);
	glutMouseFunc(bn_handler);
	glutMotionFunc(mouse_handler);
	
	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
		exit(-1);
    } 

	//textures->load("");

	// load and set the mandelbrot shader
	shaders->load("2d_fractals.vs", "2d_fractals.frag");
	setDefaultUniforms2d(shaders);
	shaders->updateValueStrings();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glutMainLoop();
}

void draw(void) {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	shaders->use();

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glutSwapBuffers();
}

void idle_handler(void) {
	glutPostRedisplay();
}

float step = 0.5f;

void key_handler(unsigned char key, int x, int y) {
	int dir = -1;
	float step_factor = 5 * camera->z;

	switch (key) {
	case 27: // ESC
	case 'q':
	case 'Q':
		exit(0);
		break;
	case GLUT_KEY_UP:
//	case 'w':
//	case 'W':
		camera->up(camera->step() * step_factor);
		break;
	case GLUT_KEY_DOWN:
//	case 's':
//	case 'S':
		camera->down(camera->step() * step_factor);
		break;
	case GLUT_KEY_PAGE_DOWN:
		camera->back(camera->step() * step_factor * dir);
		break;
	case GLUT_KEY_PAGE_UP:
		camera->forward(camera->step() * step_factor * dir);
		break;
	case GLUT_KEY_LEFT:
//	case 'a':
//	case 'A':
		camera->strafeLeft(camera->step() * step_factor);
		break;
	case GLUT_KEY_RIGHT:
//	case 'd':
//	case 'D':
		camera->strafeRight(camera->step() * step_factor);
	case '=':
		iter += 10;
		if (iter > 400) iter = 400;
		shaders->set_uniform1i("maxIterations", iter);
		shaders->updateValueStrings();
		break;
	case '-':
		iter -= 10;
		if (iter < 1) iter = 1;
		shaders->set_uniform1i("maxIterations", iter);
		shaders->updateValueStrings();
		break;
	case 'j':
	case 'J':
		shaders->toggle("juliaMode");
		break;
	default:
		break;
	}
}

int which_bn;
float px, py;

void bn_handler(int bn, int state, int x, int y) {
	int xres = glutGet(GLUT_WINDOW_WIDTH);
	int yres = glutGet(GLUT_WINDOW_HEIGHT);
	px = (float)x;
	py = (float)y;
	which_bn = bn;
	if (which_bn == 3) { // scroll up
		scale *= 1 - zoom_factor * 2.0f;
	}
	else if (which_bn == 4) { // scroll down
		scale *= 1 + zoom_factor * 2.0f;
	}
}

void mouse_handler(int x, int y) {
	int xres = glutGet(GLUT_WINDOW_WIDTH);
	int yres = glutGet(GLUT_WINDOW_HEIGHT);
	float dx = px - (float)x;
	float dy = py = (float)y;
	float deg2rad = 3.141593f / 180.0f;

	float u = dx*step, v = dy*step;

	float step_factor = 4.0f * camera->z / (float)xres;
	camera->x -= u * step_factor;
	camera->y += v * step_factor;

	px = (float)x;
	py = (float)y;
}
