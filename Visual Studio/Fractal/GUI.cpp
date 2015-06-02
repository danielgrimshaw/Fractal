/** GUI.cpp
 * Launcher program for Fractal
 * Runs a GUI for selecting settings.
 */

#include <iostream>
#include <cstdlib>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include "util.h"

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>
#include <CEGUI/System.h>

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
char filename[64] = "mbrot.glsl";

// A barebones GLUT application
int main(int argc, char ** argv) {
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

	if (!(prog = setup_shader("vector.glsl", filename))) {
		return EXIT_FAILURE;
	}
	set_uniform1i(prog, "iter", iter);
	
	glViewport(0, 0, 800, 600);

	CEGUI::OpenGL3Renderer & renderer = CEGUI::OpenGL3Renderer::bootstrapSystem();
	CEGUI::System::create(renderer);
	CEGUI::SchemeManager::getSingleton().createFromFile("WindowsLook.scheme");
	CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-10");
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("WindowsLook/MouseArrow");
	

	glutMainLoop();
	return 0;
}


void draw(void) {
	
}

void idle_handler(void) {
	glutPostRedisplay();
}

void key_handler(unsigned char key, int x, int y) {
	using namespace std;
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
		cout << "iterations: %d" << iter << endl;
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
