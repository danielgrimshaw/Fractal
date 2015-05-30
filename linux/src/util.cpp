#if defined(__unix__) || defined(unix)
#include <time.h>
#include <sys/time.h>
#else	/* assume windows */
#include <windows.h>
#endif	/* __unix__ */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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

int check_ppm(std::ifstream & fp);
void * load_ppm(std::ifstream & fp, unsigned long *xsz, unsigned long *ysz);

typedef unsigned int uint32_t;

unsigned long get_msec(void) {
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
#endif	/* __unix__ */
}

unsigned int setup_shader(const char *fname) {
	using namespace std;
	unsigned int prog, sdr, len;
	char * src_buf;
	int success, linked;
	ifstream t;

	t.open(fname);
	string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
	/*
	t.seekg(0, ios::end);
	len = t.tellg();
	t.seekg(0, ios::beg);
	src_buf = new char[len+1];
	if (src_buf == 0) {
		cout << "Unable to allocate " << len << "bytes for " << fname << endl;
		return 0;
	}
	t.read(src_buf, len);
	t.close();*/
	src_buf = new char[str.length()+1];
	src_buf = (char *)str.c_str();
	src_buf[len] = 0;

	sdr = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sdr, 1, (const char **)&src_buf, 0);
	delete [] src_buf;

	glCompileShader(sdr);
	glGetShaderiv(sdr, GL_COMPILE_STATUS, &success);
	if (!success) {
		int info_len;
		char *info_log;

		glGetShaderiv(sdr, GL_INFO_LOG_LENGTH, &info_len);
		if (info_len > 0) {
			if (!(info_log = new char[info_len + 1])) {
				cout << "Unable to allocate info_log (util.cpp: line 90)" << endl;
				return 0;
			}
			glGetShaderInfoLog(sdr, info_len, 0, info_log);
			cout << "shader compilation failed: " << info_log << endl;
			delete [] info_log;
		}
		else {
			cout << "shader compilation failed" << endl;
		}
		return 0;
	}

	prog = glCreateProgram();
	glAttachShader(prog, sdr);
	glLinkProgram(prog);
	glGetProgramiv(prog, GL_LINK_STATUS, &linked);
	if (!linked) {
		int info_len;
		char *info_log;

		glGetShaderiv(sdr, GL_INFO_LOG_LENGTH, &info_len);
		if (info_len > 0) {
			if (!(info_log = new char[info_len + 1])) {
				cout << "Unable to allocate info_log (util.cpp: line 90)" << endl;
				return 0;
			}
			glGetShaderInfoLog(sdr, info_len, 0, info_log);
			cout << "shader compilation failed: " << info_log << endl;
			delete[] info_log;
		}
		else {
			cout << "shader compilation failed" << endl;
		}
		return 0;
	}

	glUseProgram(prog);
	return prog;
}

void set_uniform1f(unsigned int prog, const char *name, float val) {
	int loc = glGetUniformLocation(prog, name);
	if (loc != -1) {
		glUniform1f(loc, val);
	}
}

void set_uniform2f(unsigned int prog, const char *name, float v1, float v2) {
	int loc = glGetUniformLocation(prog, name);
	if (loc != -1) {
		glUniform2f(loc, v1, v2);
	}
}

void set_uniform1i(unsigned int prog, const char *name, int val) {
	int loc = glGetUniformLocation(prog, name);
	if (loc != -1) {
		glUniform1i(loc, val);
	}
}

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

void * load_image(const char *fname, unsigned long *xsz, unsigned long *ysz) { // TODO: rewrite this
	using namespace std;
	ifstream fp(fname);
	if (!(fp.is_open())) {
		cout << "failed to open: " << fname << endl;
		return 0;
	}

	if (check_ppm(fp)) {
		return load_ppm(fp, xsz, ysz);
	}

	fp.close();
	cout << "unsupported image format" << endl;
	return 0;
}

int check_ppm(std::ifstream & fp) {
	using namespace std;
	fp.seekg(0, ios::beg);
	if (fp.get() == 'P' && fp.get() == '6') {
		return 1;
	}
	return 0;
}

static int read_to_wspace(std::ifstream & fp, char * buf, int bsize) {
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

void * load_ppm(std::ifstream & fp, unsigned long *xsz, unsigned long *ysz) {
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

	sz = h * w;
	for (i = 0; i<sz; i++) {
		int r = fp.get();
		int g = fp.get();
		int b = fp.get();

		if (r == -1 || g == -1 || b == -1) {
			delete [] pixels;
			fp.close();
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
