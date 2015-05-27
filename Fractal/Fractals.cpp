// STDIO
#include <iostream>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include "Shader.h"

int width = 600, height = 600; // window size
int windowID;
GLFWwindow * window;

GLfloat minX = -2.2f, maxX = 0.8f, minY = -1.5f, maxY = 1.5; // complex plane boundaries
GLfloat stepX = (maxX - minX) / (GLfloat)width;
GLfloat stepY = (maxY - minY) / (GLfloat)height;

GLfloat black[] = { 0.0f, 0.0f, 0.0f }; // black color
const int paletteSize = 128;
GLfloat palette[paletteSize][3];

const GLfloat radius = 5.0f;
bool fullScreen = false;

GLfloat* calculateColor(GLfloat u, GLfloat v){
	GLfloat re = u;
	GLfloat im = v;
	GLfloat tempRe = 0.0;
	for (int i = 0; i < paletteSize; i++){
		tempRe = re*re - im*im + u;
		im = re * im * 2 + v;
		re = tempRe;
		if ((re*re + im*im) > radius){
			return palette[i];
		}
	}
	return black;
}

void repaint() {// function called to repaint the window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen buffer
	glBegin(GL_POINTS); // start drawing in single pixel mode
	for (GLfloat y = maxY; y >= minY; y -= stepY){
		for (GLfloat x = minX; x <= maxX; x += stepX){
			glColor3fv(calculateColor(x, y)); // set color
			glVertex3f(x, y, 0.0f); // put pixel on screen (buffer)
		}
	}
	glEnd(); // end drawing
	glfwSwapBuffers(window); // swap the buffers
}

//****************************************
void reshape(int w, int h){ // function called when window size is changed
	stepX = (maxX - minX) / (GLfloat)w; // calculate new value of step along X axis
	stepY = (maxY - minY) / (GLfloat)h; // calculate new value of step along Y axis
	glViewport(0, 0, (GLsizei)w, (GLsizei)h); // set new dimension of viewable screen
	glutPostRedisplay(); // repaint the window
}

//****************************************
void keyFunction(unsigned char key, int x, int y){ // function to handle key pressing
	switch (key){
	case 'F': // pressing F is turning on/off fullscreen mode
	case 'f':
		if (fullScreen){
			glutReshapeWindow(width, height); // sets default window size
			GLsizei windowX = (glutGet(GLUT_SCREEN_WIDTH) - width) / 2;
			GLsizei windowY = (glutGet(GLUT_SCREEN_HEIGHT) - height) / 2;
			glutPositionWindow(windowX, windowY); // centers window on the screen
			fullScreen = false;
		}
		else{
			fullScreen = true;
			glutFullScreen(); // go to fullscreen mode
		}
		glutPostRedisplay();
		break;
	case 27: // escape key - close the program
		glutDestroyWindow(windowID);
		exit(0);
		break;
	}
}

//****************************************
void createPalette(){
	for (int i = 0; i < 32; i++){
		palette[i][0] = (8 * i) / (GLfloat)255;
		palette[i][1] = (128 - 4 * i) / (GLfloat)255;
		palette[i][2] = (255 - 8 * i) / (GLfloat)255;
	}
	for (int i = 0; i < 32; i++){
		palette[32 + i][0] = (GLfloat)1;
		palette[32 + i][1] = (8 * i) / (GLfloat)255;
		palette[32 + i][2] = (GLfloat)0;
	}
	for (int i = 0; i < 32; i++){
		palette[64 + i][0] = (128 - 4 * i) / (GLfloat)255;
		palette[64 + i][1] = (GLfloat)1;
		palette[64 + i][2] = (8 * i) / (GLfloat)255;
	}
	for (int i = 0; i < 32; i++){
		palette[96 + i][0] = (GLfloat)0;
		palette[96 + i][1] = (255 - 8 * i) / (GLfloat)255;
		palette[96 + i][2] = (8 * i) / (GLfloat)255;
	}
}

//****************************************
int main(int argc, char** argv){
	glutInit(&argc, argv);
	createPalette();
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	GLsizei windowX = (glutGet(GLUT_SCREEN_WIDTH) - width) / 2;
	GLsizei windowY = (glutGet(GLUT_SCREEN_HEIGHT) - height) / 2;
	glutInitWindowPosition(windowX, windowY);
	glutInitWindowSize(width, height);
	windowID = glutCreateWindow("FRAKTALE");

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(minX, maxX, minY, maxY, ((GLfloat)-1), (GLfloat)1);

	// set the event handling methods
	glutDisplayFunc(repaint);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyFunction);

	glutMainLoop();

	return 0;
}
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


	GLFWwindow * window = glfwCreateWindow(800, 600, "First OpenGL window", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetKeyCallback(window, key_callback);


}