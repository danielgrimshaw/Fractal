#include <stdio.h>
#include <stdlib.h>
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include "util.h"

void draw(void);
void mDraw(void);
void jDraw(void);
void idle_handler(void);
void mIdle_handler(void);
void jIdle_handler(void);
void key_handler(unsigned char key, int x, int y);
void bn_handler(int bn, int state, int x, int y);
void mouse_handler(int x, int y);
void mMouse_handler(int x, int y);
void jMouse_handler(int x, int y);

unsigned int prog;
float mcx = 0.7f, mcy = 0.0f, jcx, jcy;
float mscale = 2.2f;
int iter = 70;
const float mzoom_factor = 0.025f;
int fractal = 0;
int interactive = 0;

#define PX_TO_RE(x)		(1.5 * ((x) - xres / 2) / (0.5 * xres))
#define PY_TO_IM(y)		(((y) - yres / 2) / (0.5 * yres))

int main(int argc, char ** argv) {
	using namespace std;
	void * img;

	// initialize glut
	
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Fractals");

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
	if (!(prog = setup_shader("mbrot.glsl"))) {
		return EXIT_FAILURE;
	}
	set_uniform1i(prog, "iter", iter);
	cout << "Set to mandelbrot" << endl;

	glViewport(0,0,800,600);
	glutMainLoop();
	return 0;
}

void draw(void) {
	fractal == 0 ? mDraw() : jDraw();
}

void mDraw(void) {
	set_uniform2f(prog, "center", mcx, mcy);
	set_uniform1f(prog, "scale", mscale);

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

void jDraw(void) {
	if (!interactive) {
		float t = (float)get_msec() / 1000.0f;
		jcx = (sin(cos(t / 10.0f) * 10.0f) + cos(t * 2.0f) / 4.0f + sin(t * 3.0f) / 6.0f) * 0.8f;
		jcy = (cos(sin(t / 10.0f) * 10.0f) + sin(t * 2.0f) / 4.0f + cos(t * 3.0f) / 6.0f) * 0.8f;
	}
	set_uniform2f(prog, "c", jcx, jcy);

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
	fractal == 0 ? mIdle_handler() : jIdle_handler();
}

void mIdle_handler(void) {
	glutPostRedisplay();
}

void jIdle_handler(void) {
	draw();
}

void key_handler(unsigned char key, int x, int y) {
	using namespace std;
	switch (key) {
	case 27:
	case 'q':
	case 'Q':
		exit(0);
		break;
	case 'M':
	case 'm':
		fractal = 0;
		prog = setup_shader("mbrot.glsl");
		set_uniform1i(prog, "iter", iter);
		cout << "Set to mandelbrot" << endl;
		glEnable(GL_TEXTURE_1D);
		glViewport(0, 0, 800, 600);
		break;
	case 'J':
	case 'j':
		fractal = 1;
		prog = setup_shader("julia.glsl");
		cout << "Set to julia" << endl;
		glEnable(GL_TEXTURE_1D);
		glViewport(0, 0, 800, 600);
		break;
	case '+':
	case '=':
		if (1) { // allows for less code
			iter += 10;
		}
		else {
	case '_':
	case '-':
		iter -= 10;
		if (iter < 0) iter = 0;
		}
		cout << "iterations: " << iter << endl;
		set_uniform1i(prog, "iter", iter);
		break;
	case ' ':
		interactive = !interactive;
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
		mscale *= 1 - mzoom_factor * 2.0f;
	}
	else if (which_bn == 4) {
		mscale *= 1 + mzoom_factor * 2.0f;
	}
}
void mouse_handler(int x, int y) {
	fractal == 0 ? mMouse_handler(x, y) : jMouse_handler(x, y);
}

void mMouse_handler(int x, int y) {
	int xres = glutGet(GLUT_WINDOW_WIDTH);
	int yres = glutGet(GLUT_WINDOW_HEIGHT);
	float fx = 2.0f * ((float)x / (float)xres - 0.5f);
	float fy = 2.0f * ((float)y / (float)yres - 0.5f);

	if (which_bn == 1) {
		mcx += (fx - px) * mscale / 2.0f;
		mcy -= (fy - py) * mscale / 2.0f;
	}
	else if (which_bn == 0) {
		mscale *= (fy - py < 0.0) ? 1 - mzoom_factor : 1 + mzoom_factor;
	}

	px = fx;
	py = fy;
}

void jMouse_handler(int x, int y) {
	int xres = glutGet(GLUT_WINDOW_WIDTH);
	int yres = glutGet(GLUT_WINDOW_HEIGHT);
	jcx = (float)PX_TO_RE(x);
	jcy = (float)PY_TO_IM(yres - y);
}
