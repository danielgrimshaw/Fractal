#if defined(__unix__) || defined(unix)
#include <time.h>
#include <sys/time.h>
#else	// assume windows
#include <windows.h>
#endif	// __unix__

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <new>
// ^^^ so many files!

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/gl.h>
#include <SOIL.h>

#include "util.h"

static int check_ppm(std::ifstream & fp); // essentially a private method to check integrity of P6 ppm image
static void * load_ppm(std::ifstream & fp, unsigned long *xsz, unsigned long *ysz); // loads ppm image

static std::string leftText = "This is left text"; // left side control text 
static std::string rightText = "This is right text"; // right side control text
// All the controls:
static char controlsText[1024] = "ALL CONTROLS WILL ONLY WORK IF THE FRACTAL WINDOW IS THE FOCUS\r\n"
	"ESC key, Q key: Quit\r\n"
	"Scroll Wheel: Zoom in and out\r\n"
	"+ key: Increase maximum iterations\r\n"
	"- key: Decrease maximum iterations\r\n";

unsigned long get_msec(void) { // gets msec of system run time (This is just here for fun)
#if defined(__unix__) || defined(unix)
	static struct timeval timeval, first_timeval;

	gettimeofday(&timeval, 0);

	if (first_timeval.tv_sec == 0) {
		first_timeval = timeval;
		return 0;
	}
	return (timeval.tv_sec - first_timeval.tv_sec) * 1000 + (timeval.tv_usec - first_timeval.tv_usec) / 1000;
#else
	return GetTickCount();
#endif	// __unix__
}

void Shader::load(const char * vname, const char * fname) { // This is actually part of the shader class, but it is not defined withing the class
	using namespace std;
	int vs, fs, linked; // vector shader, fragment shader, error status holder
	vs = loadShader("vertex", vname); // This can use private functions, but it is not within the actual file where the class is!
	fs = loadShader("fragment", fname);

	program = glCreateProgram(); // Create GPU executable program for fractals. (Assigns it to a private var of Shader)
	glAttachShader(program, vs); // The vector shader for fractals should be included in the program
	glAttachShader(program, fs); // So should the fragment shader
	glLinkProgram(program); // Link the program
	glGetProgramiv(program, GL_LINK_STATUS, &linked); // Did it work?
	if (!linked) {
		int info_len; // No.
		char *info_log;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_len); // Get the info log
		if (info_len > 0) {
			if (!(info_log = new char[info_len + 1])) { // User needs to buy more ram! (only should have used about 1~2 MiB by now)
				cout << "Unable to allocate info_log (util.cpp: line 140)" << endl;
				exit(-2);
			}
			glGetProgramInfoLog(program, info_len, 0, info_log); // get the log
			cout << "Program linking failed: " << info_log << endl; // print the log
			delete[] info_log;
		}
		else { // OGL doesn't want to give me a log of the errors
			cout << "Program linking failed" << endl;
		}
		exit(-1); // Kill the program, it can't work without the shader sub-program
	}
}

void Shader::use() {
	glUseProgram(program); // Tell the GPU to use this program
}

/*
 * The next several functions are setters
 * I wrote this part in C originally, and
 * didn't really want to rewrite all of my
 * code. This is also the standard OpenGL
 * naming scheme (OpenGL was written for C)
 * All of the code is self explanatory
 */
void Shader::set_uniform1f(const char *name, float val) {
	int loc = glGetUniformLocation(program, name);
	if (loc != -1) {
		glUniform1f(loc, val);
	}
}

void Shader::set_uniform2f(const char *name, float v1, float v2) {
	int loc = glGetUniformLocation(program, name);
	if (loc != -1) {
		glUniform2f(loc, v1, v2);
	}
}

void Shader::set_uniform3f(const char * name, float v1, float v2, float v3) {
	int loc = glGetUniformLocation(program, name);
	if (loc != -1) {
		glUniform3f(loc, v1, v2, v3);
	}
}

void Shader::set_uniform1i(const char *name, int val) {
	int loc = glGetUniformLocation(program, name);
	if (loc != -1) {
		glUniform1i(loc, val);
	}
}

void Shader::updateValueStrings() { // Update all of the uniform values
	using namespace std;
	leftText = "";
	rightText = "";
	float fv;
	int iv = 0;

	leftText += "Type: ";
	glGetUniformiv(program, glGetUniformLocation(program, "fractal"), &iv);
	leftText += iv == 0 ? "Mandelbrot" : iv == 1 ? "Orbit Trap" : "Ducks";
	leftText += "\r\n";

	leftText += "Max Iterations: ";
	glGetUniformiv(program, glGetUniformLocation(program, "maxIterations"), &iv);
	leftText += to_string(iv);
	leftText += "\r\n";

	leftText += "Antialiasing? ";
	glGetUniformiv(program, glGetUniformLocation(program, "antialiasingOn"), &iv);
	leftText += iv == 0 ? "off" : "on";
	leftText += "\r\n";

	leftText += "Scale: ";
	glGetUniformfv(program, glGetUniformLocation(program, "scale"), &fv);
	leftText += to_string(fv);
	leftText += "\r\n";

	leftText += "Power: ";
	glGetUniformfv(program, glGetUniformLocation(program, "power"), &fv);
	leftText += to_string(fv);
	leftText += "\r\n";

	leftText += "Bailout value: ";
	glGetUniformfv(program, glGetUniformLocation(program, "bailout"), &fv);
	leftText += to_string(fv);
	leftText += "\r\n";

	leftText += "Min iterations: ";
	glGetUniformiv(program, glGetUniformLocation(program, "minIterations"), &iv);
	leftText += to_string(iv);
	leftText += "\r\n";

	leftText += "Julia mode? ";
	glGetUniformiv(program, glGetUniformLocation(program, "juliaMode"), &iv);
	leftText += iv == 0 ? "off" : "on";
	leftText += "\r\n";

	leftText += "Offset: ";
	glGetUniformfv(program, glGetUniformLocation(program, "power"), &fv);
	leftText += to_string(fv);
	leftText += "\r\n";

	leftText += "Color Mode: ";
	glGetUniformiv(program, glGetUniformLocation(program, "colorMode"), &iv);
	leftText += to_string(iv);
	leftText += "\r\n";

	leftText += "Bailout style: ";
	glGetUniformiv(program, glGetUniformLocation(program, "bailoutStyle"), &iv);
	leftText += to_string(iv);
	leftText += "\r\n";

	leftText += "Color scale: ";
	glGetUniformfv(program, glGetUniformLocation(program, "colorScale"), &fv);
	leftText += to_string(fv);
	leftText += "\r\n";

	leftText += "Color cycle: ";
	glGetUniformfv(program, glGetUniformLocation(program, "colorCycle"), &fv);
	leftText += to_string(fv);
	leftText += "\r\n";

	leftText += "Color cycle offset: ";
	glGetUniformfv(program, glGetUniformLocation(program, "colorCycleOffset"), &fv);
	leftText += to_string(fv);
	leftText += "\r\n";

	leftText += "Mirror colors? ";
	glGetUniformiv(program, glGetUniformLocation(program, "colorCycleMirror"), &iv);
	leftText += iv == 0 ? "off" : "on";
	leftText += "\r\n";

	leftText += "Rainbow mode? ";
	glGetUniformiv(program, glGetUniformLocation(program, "hsv"), &iv);
	leftText += iv == 0 ? "off" : "on";
	leftText += "\r\n";

	leftText += "Iteration color blend: ";
	glGetUniformfv(program, glGetUniformLocation(program, "iterationColorBlend"), &fv);
	leftText += to_string(fv);
	leftText += "\r\n";

	leftText += "Color iterations: ";
	glGetUniformiv(program, glGetUniformLocation(program, "colorIterations"), &iv);
	leftText += to_string(iv);
	leftText += "\r\n";

	leftText += "Gamma correction: ";
	glGetUniformfv(program, glGetUniformLocation(program, "gamma"), &fv);
	leftText += to_string(fv);
	leftText += "\r\n";

	leftText += "Rotation: ";
	glGetUniformfv(program, glGetUniformLocation(program, "rotation"), &fv);
	leftText += to_string(fv);
	leftText += "\r\n";
}

GLint Shader::getAttribLocation(const char * name) {
	return glGetAttribLocation(program, name);
}

void Shader::toggle(const char * name) {
	int val;
	glGetUniformiv(program, glGetUniformLocation(program, name), &val);
	set_uniform1i(name, val == 1 ? 0 : 1);
}

void setDefaultUniforms2d(Shader * shaders) { // Sets all of the defaults for 2D fractals
	shaders->set_uniform1i("fractal", MANDELBROT); // Fractal type

	shaders->set_uniform1i("maxIterations", 50);
	shaders->set_uniform1i("antialiasingOn", 0);

	shaders->set_uniform1f("scale", 2.0f);
	shaders->set_uniform1f("power", 2.0f);
	shaders->set_uniform1f("bailout", 4.0f);
	shaders->set_uniform1i("minIterations", 1);

	shaders->set_uniform1i("juliaMode", 0);
	shaders->set_uniform2f("offset", 0.36f, 0.06f);

	shaders->set_uniform1i("colorMode", 0);
	shaders->set_uniform1i("bailoutStyle", 0);
	shaders->set_uniform1f("colorScale", 1.0f);
	shaders->set_uniform1f("colorCycle", 1.0f);
	shaders->set_uniform1f("colorCycleOffset", 0.0f);
	shaders->set_uniform1i("colorCycleMirror", 1);
	shaders->set_uniform1i("hsv", 0);
	shaders->set_uniform1f("iterationColorBlend", 0.0f);

	shaders->set_uniform1i("colorIterations", 4);
	shaders->set_uniform3f("color1", 1.0f, 1.0f, 1.0f);
	shaders->set_uniform3f("color2", 0.0f, 0.53f, 0.8f);
	shaders->set_uniform3f("color3", 0.0f, 0.0f, 0.0f);
	shaders->set_uniform1i("transparent", 0);
	shaders->set_uniform1f("gamma", 1.0f);

	shaders->set_uniform1i("orbitTrap", 0);
	shaders->set_uniform2f("orbitTrapOffset", 0.0f, 0.0f);
	shaders->set_uniform1f("orbitTrapScale", 1.0f);
	shaders->set_uniform1f("orbitTrapEdgeDetail", 0.5f);
	shaders->set_uniform1f("orbitTrapRotation", 0.0f);
	shaders->set_uniform1f("orbitTrapSpin", 0.0f);
	//shaders->set_uniform1i("texture", 0);

	shaders->set_uniform1f("rotation", 0.0f);
	shaders->set_uniform3f("cameraPosition", -0.5f, 0.0f, 2.5f);
	shaders->set_uniform2f("size", 400.0f, 300.0f);
	shaders->set_uniform2f("outputSize", 800.0f, 600.0f);
}

void resize(unsigned int width, unsigned int height) {
	glViewport(0, 0, width, height);
}

uint8_t * getPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	uint8_t * pixels = new uint8_t[width * height * 4];
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	return pixels;
}

void Texture::load(const char * path) {
	// Load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	int width, height;
	unsigned char * image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
}

PCTSTR getLeftStrings() { // return the left side text
	return leftText.c_str();
}

PCTSTR getRightStrings() { // return the right side text
	return rightText.c_str();
}

PCTSTR getControls() { // return the controls list
	return controlsText;
}