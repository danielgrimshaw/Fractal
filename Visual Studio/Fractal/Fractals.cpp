#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <cstdlib>
#include "util.h"
/*
void draw(void);
void idle_handler(void);
void key_handler(unsigned char key, int x, int y);
void bn_handler(int bn, int state, int x, int y);
void mouse_handler(int x, int y);

unsigned int prog;
float cx = 0.7f, cy = 0.0f;
float scale = 2.2f;
int iter = 70;
const float zoom_factor = 0.025f;

int main(int argc, char ** argv) {
	void * img;

	// initialize glut
	
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Mandelbrot");

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
        return -1;
    } 

	// load the 1D palette texture
	glBindTexture(GL_TEXTURE_1D, 1);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	if (!(img = load_image("pal.ppm", 0, 0))) {
		return EXIT_FAILURE;
	}
	glTexImage1D(GL_TEXTURE_1D, 0, 4, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, img);
	delete img;

	glEnable(GL_TEXTURE_1D);

	// load and set the mandelbrot shader
	if (!(prog = setup_shader("vertex.glsl", "mbrot.glsl"))) {
		return EXIT_FAILURE;
	}
	set_uniform1i(prog, "iter", iter);

	glViewport(0,0,800,600);
	glutMainLoop();
	return 0;
}

void draw(void) {
	set_uniform2f(prog, "center", cx, cy);
	set_uniform1f(prog, "scale", scale);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glEnd();

	glutSwapBuffers();
}

void idle_handler(void) {
	glutPostRedisplay();
}

void key_handler(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC
	case 'q':
	case 'Q':
		exit(0);

	case '=':
		iter += 10;
	case '-':
		iter -= 10;
		if (iter < 0) iter = 0;
		set_uniform1i(prog, "iter", iter);
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
	px = 2.0f * ((float)x / (float)xres - 0.5f);
	py = 2.0f * ((float)y / (float)yres - 0.5f);
	which_bn = bn;

	if (which_bn == 3) {
		scale *= 1 - zoom_factor * 2.0f;
	}
	else if (which_bn == 4) {
		scale *= 1 + zoom_factor * 2.0f;
	}
}

void mouse_handler(int x, int y) {
	int xres = glutGet(GLUT_WINDOW_WIDTH);
	int yres = glutGet(GLUT_WINDOW_HEIGHT);
	float fx = 2.0f * ((float)x / (float)xres - 0.5f);
	float fy = 2.0f * ((float)y / (float)yres - 0.5f);

	if (which_bn == 1) {
		cx += (fx - px) * scale / 2.0f;
		cy -= (fy - py) * scale / 2.0f;
	}
	else if (which_bn == 0) {
		scale *= (fy - py < 0.0) ? 1 - zoom_factor : 1 + zoom_factor;
	}

	px = fx;
	py = fy;
}
*/