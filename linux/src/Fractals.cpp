#include <stdio.h>
#include <stdlib.h>
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include "util.h"

void draw(void); // Redraw handler
void mDraw(void); // Mandelbrot redraw handler
void jDraw(void); // Julia redraw handler
void idle_handler(void); // Idle handler
void mIdle_handler(void); // Mandelbrot idle handler
void jIdle_handler(void); // Julia idle handler
void key_handler(unsigned char key, int x, int y); // keyboard driver
void bn_handler(int bn, int state, int x, int y); // Mouse button handler (Mandelbrot only)
void mouse_handler(int x, int y); // Mouse motion handler
void mMouse_handler(int x, int y); // Mandelbrot mouse motion handler
void jMouse_handler(int x, int y); // Julia mouse motion handler

unsigned int prog; // Program ID
float mcx = 0.7f, mcy = 0.0f, jcx, jcy; // C values
float mscale = 2.2f; // Mandelbrot scaling factor
int iter = 70; // Bailout iterations (Most GPU's can manage about 750 iterations before they start to lag)
const float mzoom_factor = 0.025f; // Mandelbrot zoom
int fractal = 0; // Fractal ID (0 == Mandelbrot, 1 == Julia)
int interactive = 0; // Julia interactive mode
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

#define PX_TO_RE(x)		(1.5 * ((x) - xres / 2) / (0.5 * xres))
#define PY_TO_IM(y)		(((y) - yres / 2) / (0.5 * yres))

int main(int argc, char ** argv) {
	using namespace std;
	void * img;

	// Print controls
	cout << controls << endl;

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
	
	// Load all library function pointers
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { // Doesn't run without GLEW
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    } 

	// load the 1D palette texture
	glBindTexture(GL_TEXTURE_1D, 1);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	if (!(img = load_image("pal.ppm", 0, 0))) { // pal.ppm is used for texture
		return EXIT_FAILURE;
	}
	glTexImage1D(GL_TEXTURE_1D, 0, 4, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, img); // Load image to GPU
	delete img; // CPU ram no longer needs image

	glEnable(GL_TEXTURE_1D); // Turn on the texture

	// load and set the shader
	if (!(prog = setup_shader("mbrot.glsl"))) {
		return EXIT_FAILURE;
	}
	set_uniform1i(prog, "iter", iter); // Tell shader that we are currently doing 70 iterations
	cout << "Set to mandelbrot" << endl;

	glViewport(0,0,800,600); // Tell GPU where to draw to and window size
	glutMainLoop(); // Enter callback loop
	return 0;
}

void draw(void) {
	fractal == 0 ? mDraw() : jDraw(); // Call Mandelbrot or Julia draw methods
}

void mDraw(void) {
	set_uniform2f(prog, "center", mcx, mcy); // Set center
	set_uniform1f(prog, "scale", mscale); // Set scale

	glBegin(GL_QUADS); // Draw rectangular window to write to
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
	if (!interactive) { // Animate
		float t = (float)get_msec() / 1000.0f;
		jcx = (sin(cos(t / 10.0f) * 10.0f) + cos(t * 2.0f) / 4.0f + sin(t * 3.0f) / 6.0f) * 0.8f;
		jcy = (cos(sin(t / 10.0f) * 10.0f) + sin(t * 2.0f) / 4.0f + cos(t * 3.0f) / 6.0f) * 0.8f;
	}
	set_uniform2f(prog, "c", jcx, jcy);

	glBegin(GL_QUADS); // Draw window to write to
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
	glutPostRedisplay(); // Fractal doesn't need to draw while waiting
}

void jIdle_handler(void) {
	draw(); // Julia needs to draw for animations
}

void key_handler(unsigned char key, int x, int y) {
	using namespace std;
	switch (key) { // Keyboard
	case 27: // ESC
	case 'q':
	case 'Q': // Quit
		exit(0);
		break;
	case 'c':
	case 'C':
	case 'h':
	case 'H':
	case '/':
	case '?': // help
		cout << controls << endl;
		break;
	case 'M':
	case 'm': // Switch to mandelbrot mode
		fractal = 0;
		prog = setup_shader("mbrot.glsl");
		set_uniform1i(prog, "iter", iter);
		cout << "Set to mandelbrot" << endl;
		glEnable(GL_TEXTURE_1D);
		glViewport(0, 0, 800, 600);
		break;
	case 'J':
	case 'j': // Switch to Julia mode
		fractal = 1;
		prog = setup_shader("julia.glsl");
		set_uniform1i(prog, "iter", iter);
		cout << "Set to julia" << endl;
		glEnable(GL_TEXTURE_1D);
		glViewport(0, 0, 800, 600);
		break;
	case '+':
	case '=': // Increase iterations
		if (1) { // allows for less code
			iter += 10;
		}
		else {
	case '_':
	case '-': // Decrease iterations
		iter -= 10;
		if (iter < 0) iter = 0;
		}
		cout << "Iterations: " << iter << endl;
		set_uniform1i(prog, "iter", iter);
		break;
	case ' ': // Toggle julia interactive mode
		interactive = !interactive;
		break;
	default: // No key was pressed that is of importance
		break;
	}
}

int which_bn; // Button id
float px, py;

void bn_handler(int bn, int state, int x, int y) {
	int xres = glutGet(GLUT_WINDOW_WIDTH);
	int yres = glutGet(GLUT_WINDOW_HEIGHT);
	px = 2.0f * ((float)x / (float)xres - 0.5f);
	py = 2.0f * ((float)y / (float)yres - 0.5f);
	which_bn = bn;

	if (which_bn == 3) { // Scroll up
		mscale *= 1 - mzoom_factor * 2.0f;
	}
	else if (which_bn == 4) { // Scroll down
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
	if (interactive) { // If interactive mode is on, set C to window coords
		jcx = (float)PX_TO_RE(x);
		jcy = (float)PY_TO_IM(yres - y);
	}
}
