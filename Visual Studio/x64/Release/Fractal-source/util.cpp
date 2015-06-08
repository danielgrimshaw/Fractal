#if defined(__unix__) || defined(unix)
#include <time.h>
#include <sys/time.h>
#else	// assume windows
#include <windows.h>
#endif	// __unix__

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <fstream>
#include <new>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/gl.h>

#include "util.h"

int check_ppm(std::ifstream & fp); // Checks the validity of a P6 ppm image file
void * load_ppm(std::ifstream & fp, unsigned long *xsz, unsigned long *ysz); // Loads a P6 image file

unsigned long get_msec(void) { // Returns system run time
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

unsigned int setup_shader(const char *fname) { // Loads, compiles, and tells GPU to use shader
	using namespace std;
	unsigned int prog, sdr; // OpenGL IDs
	char * src_buf; // Code buffer 2
	int success, linked; // Success flags
	string str; // Code buffer one
	ifstream t; // File buffer

	t.open(fname);
	t.seekg(0, std::ios::end); // Go to the end
	str.reserve(t.tellg()); // Read location (length of the file) and reserve a string that big
	t.seekg(0, std::ios::beg); // Go back to the beginning

	str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>()); // Assign the entire file to str

	src_buf = new char[str.length() + 1]; // Allocate memory for C-style string
	src_buf = (char *)str.c_str(); // Assign string
	src_buf[str.length()] = 0; // Doesn't work without this for some reason
	t.close();

	sdr = glCreateShader(GL_FRAGMENT_SHADER); // Create a fragment shader
	glShaderSource(sdr, 1, (const char **)&src_buf, 0); // Use src_buf as the code
	//delete[] src_buf; // Doesn't work for some reason

	glCompileShader(sdr); // Compile shader
	glGetShaderiv(sdr, GL_COMPILE_STATUS, &success); // Did it work?
	if (!success) {
		int info_len; // No.
		char *info_log;

		glGetShaderiv(sdr, GL_INFO_LOG_LENGTH, &info_len); // Read size of log
		if (info_len > 0) {
			if (!(info_log = new char[info_len + 1])) { // User needs significantly more RAM
				cout << "Unable to allocate info_log (util.cpp: line 90)" << endl;
				return 0;
			}
			glGetShaderInfoLog(sdr, info_len, 0, info_log); // Get info log
			cout << "shader compilation failed: " << info_log << endl;
			delete[] info_log;
		}
		else { // There is no info log?
			cout << "shader compilation failed" << endl;
		}
		return 0;
	}

	prog = glCreateProgram(); // Allocate space on the GPU for a program
	glAttachShader(prog, sdr); // Program needs the compiled code
	glLinkProgram(prog); // Makes the program executable
	glGetProgramiv(prog, GL_LINK_STATUS, &linked); // Did it work?
	if (!linked) {
		int info_len; // no
		char *info_log;

		glGetShaderiv(sdr, GL_INFO_LOG_LENGTH, &info_len); // Get info log size
		if (info_len > 0) {
			if (!(info_log = new char[info_len + 1])) { // User needs more RAM
				cout << "Unable to allocate info_log (util.cpp: line 90)" << endl;
				return 0;
			}
			glGetShaderInfoLog(sdr, info_len, 0, info_log); // Read info log
			cout << "shader compilation failed: " << info_log << endl;
			delete[] info_log;
		}
		else { // There was no info log?
			cout << "shader compilation failed" << endl;
		}
		return 0;
	}

	glUseProgram(prog); // Instruct the GPU to use this program
	return prog; // returns the program ID
}

void set_uniform1f(unsigned int prog, const char *name, float val) { // Sets float uniforms
	int loc = glGetUniformLocation(prog, name);
	if (loc != -1) {
		glUniform1f(loc, val);
	}
}

void set_uniform2f(unsigned int prog, const char *name, float v1, float v2) { // Sets vec2 uniforms
	int loc = glGetUniformLocation(prog, name);
	if (loc != -1) {
		glUniform2f(loc, v1, v2);
	}
}

void set_uniform1i(unsigned int prog, const char *name, int val) { // Sets int and bool uniforms
	int loc = glGetUniformLocation(prog, name);
	if (loc != -1) {
		glUniform1i(loc, val);
	}
}

//How does the system run?
#if !defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)
#if  defined(__i386__) || defined(__ia64__) || defined(WIN32) || \
    (defined(__alpha__) || defined(__alpha)) || \
     defined(__arm__) || \
    (defined(__mips__) && defined(__MIPSEL__)) || \
     defined(__SYMBIAN32__) || \
     defined(__x86_64__) || \
     defined(__LITTLE_ENDIAN__)
/* little endian */
#define LITTLE_ENDIAN
#else
/* big endian */
#define BIG_ENDIAN
#endif	/* endian check */
#endif	/* !defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN) */

#ifdef LITTLE_ENDIAN
#define PACK_COLOR24(r, g, b) (((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff))
#else
#define PACK_COLOR24(r, g, b) (((b & 0xff) << 16) | ((g & 0xff) << 8) | (r & 0xff))
#endif

void * load_image(const char *fname, unsigned long *xsz, unsigned long *ysz) { // Load a ppm image
	using namespace std;
	ifstream fp(fname);
	if (!(fp.is_open())) { // Cannot open file
		cout << "failed to open: " << fname << endl;
		return 0;
	}

	if (check_ppm(fp)) { // Is it a P6 ppm?
		return load_ppm(fp, xsz, ysz); // Open the image
	}

	fp.close();
	cout << "unsupported image format" << endl; // It is not a P6 ppm image
	return 0;
}

int check_ppm(std::ifstream & fp) {
	using namespace std;
	fp.seekg(0, ios::beg); // Go to the begining
	if (fp.get() == 'P' && fp.get() == '6') {
		return 1; // It is a P6 ppm
	}
	return 0; // It is not a P6 ppm
}

static int read_to_wspace(std::ifstream & fp, char * buf, int bsize) { // Read image until whitespace
	int count = 0;
	char c;

	while (fp.get(c) && !isspace(c) && count < bsize - 1) {
		if (c == '#') {
			while (fp.get(c) && c != '\n' && c != '\r');
			c = fp.get();
			if (c == '\n' || c == '\r') continue;
		}
		*buf++ = c;
		count++;
	}
	*buf = 0;

	while (fp.get(c) && isspace(c));
	fp.putback(c);
	return count;
}

void * load_ppm(std::ifstream & fp, unsigned long *xsz, unsigned long *ysz) { // Load P6 PPM
	using namespace std;
	char buf[64];
	int bytes, raw;
	unsigned int w, h, i, sz;
	uint32_t *pixels;

	fp.seekg(0, ios::beg);

	bytes = read_to_wspace(fp, buf, 64);
	raw = buf[1] == '6';

	if ((bytes = read_to_wspace(fp, buf, 64)) == 0) {
		fp.close();
		return 0;
	}
	if (!isdigit(*buf)) {
		cout << "load_ppm: invalid width: " << buf;
		fp.close();
		return 0;
	}
	w = atoi(buf);

	if ((bytes = read_to_wspace(fp, buf, 64)) == 0) {
		fp.close();
		return 0;
	}
	if (!isdigit(*buf)) {
		cout << "load_ppm: invalid height: " << buf;
		fp.close();
		return 0;
	}
	h = atoi(buf);

	if ((bytes = read_to_wspace(fp, buf, 64)) == 0) {
		fp.close();
		return 0;
	}
	if (!isdigit(*buf) || atoi(buf) != 255) {
		cout << "load_ppm: invalid or unsupported max value: " << buf;
		fp.close();
		return 0;
	}

	if (!(pixels = new uint32_t[w * h])) {
		cout << "malloc failed";
		fp.close();
		return 0;
	}

	sz = h * w; // size of image
	for (i = 0; i < sz; i++) {
		int r = fp.get();
		int g = fp.get();
		int b = fp.get();

		if (r == -1 || g == -1 || b == -1) {
			delete [] pixels;
			fp.close(); // Image is corrupt
			cout << "load_ppm: EOF while reading pixel data";
			return 0;
		}
		pixels[i] = PACK_COLOR24(r, g, b);
	}

	fp.close();

	if (xsz) *xsz = w;
	if (ysz) *ysz = h;
	return pixels;
}
