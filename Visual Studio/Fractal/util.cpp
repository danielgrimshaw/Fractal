/*
 * util.c
 * 
 * Copyright 2015  <Daniel Grimshaw>
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

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/gl.h>

#include "util.h"

int check_ppm(FILE * fp); // Checks the validity of a P6 ppm image file
void * load_ppm(FILE * fp, unsigned long *xsz, unsigned long *ysz); // Loads a P6 image file

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

unsigned int setup_shader(const char * vertex_fname,
		const char * fragment_fname) {
	// Loads, compiles, and tells GPU to use shaders
	unsigned int prog, vsdr, fsdr; // OpenGL IDs
	int linked; // Success flags

	if ((vsdr = compile_vertex(vertex_fname)) == 0) {
		fprintf(stderr, "Dectected error while compiling vertex shader %s!\n",
				vertex_fname);
		return 0;
	}

	if ((fsdr = compile_fragment(fragment_fname)) == 0) {
		fprintf(stderr, "Dectected error while compiling fragment shader %s!\n",
				fragment_fname);
		return 0;
	}

	prog = glCreateProgram(); // Allocate space on the GPU for a program
	glAttachShader(prog, vsdr); // Program needs the compiled code
	glAttachShader(prog, fsdr);
	glLinkProgram(prog); // Makes the program executable
	glGetProgramiv(prog, GL_LINK_STATUS, &linked); // Did it work?
	if (!linked) {
		int info_len; // no
		char * info_log;

		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &info_len); // Get info log size
		if (info_len > 0) {
			if (!(info_log = new char[info_len + 1])) { // User needs more RAM
				fprintf(stderr,
						"Unable to allocate info_log (util.cpp: line 90)\n");
				return 0;
			}
			glGetProgramInfoLog(prog, info_len, 0, info_log); // Read info log
			fprintf(stderr, "Program linking failed: %s\n", info_log);
			delete[] info_log;
		} else { // There was no info log?
			fprintf(stderr, "Program linking failed");
		}
		return 0;
	}

	glUseProgram(prog); // Instruct the GPU to use this program
	return prog; // returns the program ID
}

int compile_vertex(const char * vertex_fname) {
	// Compile vertex shader and return ID
	int vsdr, success;
	unsigned int size;
	char * src_buf;
	FILE * buffer; // Vertex shader buffer

	buffer = fopen(vertex_fname, "r");
	if (buffer == NULL) {
		fprintf(stderr, "Unable to read vertex shader %s!\n", vertex_fname);
		return 0;
	}
	fseek(buffer, 0, SEEK_END);
	size = ftell(buffer);
	rewind(buffer);

	src_buf = new char[size + 1];
	fread(src_buf, size, 1, buffer);
	if (fclose(buffer) != 0) {
		printf("Unable to close vertex shader %s!\n", vertex_fname);
		return 0;
	}

	src_buf[size] = 0;
	printf(src_buf);

	vsdr = glCreateShader(GL_VERTEX_SHADER); // Create a vertex shader
	glShaderSource(vsdr, 1, (const char **) &src_buf, 0); // Use src_buf as the code

	glCompileShader(vsdr); // Compile shader
	glGetShaderiv(vsdr, GL_COMPILE_STATUS, &success); // Did it work?
	if (!success) {
		int info_len; // No.
		char *info_log;

		glGetShaderiv(vsdr, GL_INFO_LOG_LENGTH, &info_len); // Read size of log
		if (info_len > 0) {
			if (!(info_log = new char[info_len + 1])) { // User needs significantly more RAM
				fprintf(stderr,
						"Unable to allocate info_log (util.c: line 73)\n");
				return 0;
			}
			glGetShaderInfoLog(vsdr, info_len, 0, info_log); // Get info log
			fprintf(stderr, "Vertex shader compilation failed: %s\n", info_log);
			delete[] info_log;
		} else { // There is no info log?
			fprintf(stderr, "Vertex shader compilation failed");
		}
		return 0;
	}

	return vsdr;
}

int compile_fragment(const char * fragment_fname) {
	// Compile Fragment shader and return ID
	int fsdr, success;
	unsigned int size;
	char * src_buf;
	FILE * buffer; // Fragment shader buffer

	buffer = fopen(fragment_fname, "r");
	if (buffer == NULL) {
		fprintf(stderr, "Unable to find fragment shader %s!\n", fragment_fname);
		return 0;
	}
	fseek(buffer, 0, SEEK_END);
	size = ftell(buffer);
	rewind(buffer);

	src_buf = new char[size];
	fread(src_buf, sizeof(char), size, buffer);
	if (fclose(buffer) != 0) {
		printf("Unable to close fragment shader %s!\n", fragment_fname);
		return 0;
	}

	src_buf[size] = '\0';

	fsdr = glCreateShader(GL_FRAGMENT_SHADER); // Create a vertex shader
	glShaderSource(fsdr, 1, (const char **) &src_buf, 0); // Use src_buf as the code
	delete[] src_buf;

	glCompileShader(fsdr); // Compile shader
	glGetShaderiv(fsdr, GL_COMPILE_STATUS, &success); // Did it work?
	if (!success) {
		int info_len; // No.
		char * info_log;

		glGetShaderiv(fsdr, GL_INFO_LOG_LENGTH, &info_len); // Read size of log
		if (info_len > 0) {
			if (!(info_log = new char[info_len + 1])) { // User needs significantly more RAM
				fprintf(stderr,
						"Unable to allocate info_log (util.c: line 73)\n");
				return 0;
			}
			glGetShaderInfoLog(fsdr, info_len, 0, info_log); // Get info log
			fprintf(stderr, "Fragment shader compilation failed: %s\n",
					info_log);
			delete[] info_log;
		} else { // There is no info log?
			fprintf(stderr, "Fragment shader compilation failed");
		}
		return 0;
	}

	return fsdr;
}

void set_uniform1f(unsigned int prog, const char * name, float val) { // Sets float uniforms
	int loc = glGetUniformLocation(prog, name);
	if (loc != -1) {
		glUniform1f(loc, val);
	}
}

void set_uniform2f(unsigned int prog, const char * name, float v1, float v2) { // Sets vec2 uniforms
	int loc = glGetUniformLocation(prog, name);
	if (loc != -1) {
		glUniform2f(loc, v1, v2);
	}
}

void set_uniform1i(unsigned int prog, const char * name, int val) { // Sets int and bool uniforms
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

void * load_image(const char * fname, unsigned long * xsz,
		unsigned long * ysz) { // Load a ppm image
	FILE * fp = fopen(fname, "r");
	if (fp == NULL) { // Cannot open file
		fprintf(stderr, "failed to open: %s\n", fname);
		return 0;
	}

	if (check_ppm(fp)) { // Is it a P6 ppm?
		return load_ppm(fp, xsz, ysz); // Open the image
	}

	fclose(fp);
	fprintf(stderr, "unsupported image format\n"); // It is not a P6 ppm image
	return 0;
}

char * read_file(const char * fname) {
	FILE * fp = fopen(fname, "r");
	int size;
	char * data;
	if (fp == NULL) {
		fprintf(stderr, "failed to open: %s\n", fname);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);

	data = new char[size];
	fread(data, sizeof(char), size, fp);
	fclose(fp);

	return data;
}

int check_ppm(FILE * fp) {
	rewind(fp);
	if (fgetc(fp) == 'P' && fgetc(fp) == '6') {
		return 1; // It is a P6 ppm
	}
	return 0; // It is not a P6 ppm
}

static int read_to_wspace(FILE * fp, char * buf, int bsize) { // Read image until whitespace
	int count = 0;
	char c;

	while (c = fgetc(fp) && !isspace(c) && count < bsize - 1) {
		if (c == '#') {
			while (c = fgetc(fp) && c != '\n' && c != '\r')
				;
			c = fgetc(fp);
			if (c == '\n' || c == '\r')
				continue;
		}
		*buf++ = c;
		count++;
	}
	*buf = 0;

	while (c = fgetc(fp) && isspace(c))
		;
	fputc(c, fp);
	return count;
}

void * load_ppm(FILE * fp, unsigned long * xsz, unsigned long * ysz) { // Load P6 PPM
	char buf[64];
	int bytes, raw;
	unsigned int w, h, i, sz;
	uint32_t * pixels;

	rewind(fp);

	bytes = read_to_wspace(fp, buf, 64);
	raw = buf[1] == '6';

	if ((bytes = read_to_wspace(fp, buf, 64)) == 0) {
		fclose(fp);
		return 0;
	}
	if (!isdigit(*buf)) {
		fprintf(stderr, "load_ppm: invalid width: %s\n", buf);
		fclose(fp);
		return 0;
	}
	w = atoi(buf);

	if ((bytes = read_to_wspace(fp, buf, 64)) == 0) {
		fclose(fp);
		return 0;
	}
	if (!isdigit(*buf)) {
		fprintf(stderr, "load_ppm: invalid height: %s\n", buf);
		fclose(fp);
		return 0;
	}
	h = atoi(buf);

	if ((bytes = read_to_wspace(fp, buf, 64)) == 0) {
		fclose(fp);
		return 0;
	}
	if (!isdigit(*buf) || atoi(buf) != 255) {
		fprintf(stderr, "load_ppm: invalid or unsupported max value: %s\n",
				buf);
		fclose(fp);
		return 0;
	}

	if (!(pixels = new uint32_t[w * h])) {
		fprintf(stderr, "Memory allocation failed\n");
		fclose(fp);
		return 0;
	}

	sz = h * w; // size of image
	for (i = 0; i < sz; i++) {
		int r = fgetc(fp);
		int g = fgetc(fp);
		int b = fgetc(fp);

		if (r == -1 || g == -1 || b == -1) {
			delete[] pixels;
			fclose(fp); // Image is corrupt
			fprintf(stderr, "load_ppm: EOF while reading pixel data\n");
			return 0;
		}
		pixels[i] = PACK_COLOR24(r, g, b);
	}

	fclose(fp);

	if (xsz)
		*xsz = w;
	if (ysz)
		*ysz = h;
	return pixels;
}
