#if defined(__unix__) || defined(unix)
#include <time.h>
#include <sys/time.h>
#else	/* assume windows */
#include <windows.h>
#endif	/* __unix__ */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/gl.h>

#include "util.h"

#ifdef __unix__
GLhandleARB glCreateShaderObjectARB(GLenum);
void glShaderSourceARB(GLhandleARB, int, const char**, int*);
void glCompileShaderARB(GLhandleARB);
GLhandleARB glCreateProgramObjectARB(void);
void glAttachObjectARB(GLhandleARB, GLhandleARB);
void glLinkProgramARB(GLhandleARB);
void glUseProgramObjectARB(GLhandleARB);
void glGetInfoLogARB(GLhandleARB, GLsizei, GLsizei*, GLcharARB*);
void glGetObjectParameterivARB(GLhandleARB, GLenum, int*);
GLint glGetUniformLocationARB(GLhandleARB, const char*);
void glUniform1f(GLint location, GLfloat v0);
void glUniform1i(GLint location, GLint v0);
void glUniform2f(GLint location, GLfloat v0, GLfloat v1);
#endif


int check_ppm(FILE *fp);
void *load_ppm(FILE *fp, unsigned long *xsz, unsigned long *ysz);

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
	FILE * fp;
	unsigned int prog, sdr, len;
	char *src_buf;
	int success, linked;

	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "failed to open shader: %s\n", fname);
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	src_buf = malloc(len + 1);

	fread(src_buf, 1, len, fp);
	src_buf[len] = 0;

	sdr = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	glShaderSourceARB(sdr, 1, (const char**)&src_buf, 0);
	free(src_buf);

	glCompileShaderARB(sdr);
	glGetObjectParameterivARB(sdr, GL_OBJECT_COMPILE_STATUS_ARB, &success);
	if (!success) {
		int info_len;
		char *info_log;

		glGetObjectParameterivARB(sdr, GL_OBJECT_INFO_LOG_LENGTH_ARB, &info_len);
		if (info_len > 0) {
			if (!(info_log = malloc(info_len + 1))) {
				perror("malloc failed");
				return 0;
			}
			glGetInfoLogARB(sdr, info_len, 0, info_log);
			fprintf(stderr, "shader compilation failed: %s\n", info_log);
			free(info_log);
		}
		else {
			fprintf(stderr, "shader compilation failed\n");
		}
		return 0;
	}

	prog = glCreateProgramObjectARB();
	glAttachObjectARB(prog, sdr);
	glLinkProgramARB(prog);
	glGetObjectParameterivARB(prog, GL_OBJECT_LINK_STATUS_ARB, &linked);
	if (!linked) {
		fprintf(stderr, "shader linking failed\n");
		return 0;
	}

	glUseProgramObjectARB(prog);
	return prog;
}

void set_uniform1f(unsigned int prog, const char *name, float val) {
	int loc = glGetUniformLocationARB(prog, name);
	if (loc != -1) {
		glUniform1f(loc, val);
	}
}

void set_uniform2f(unsigned int prog, const char *name, float v1, float v2) {
	int loc = glGetUniformLocationARB(prog, name);
	if (loc != -1) {
		glUniform2f(loc, v1, v2);
	}
}

void set_uniform1i(unsigned int prog, const char *name, int val) {
	int loc = glGetUniformLocationARB(prog, name);
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

void *load_image(const char *fname, unsigned long *xsz, unsigned long *ysz) {
	FILE *fp = fopen(fname, "r");
	if (!fp) {
		fprintf(stderr, "failed to open: %s\n", fname);
		return 0;
	}

	if (check_ppm(fp)) {
		return load_ppm(fp, xsz, ysz);
	}

	fclose(fp);
	fprintf(stderr, "unsupported image format\n");
	return 0;
}

int check_ppm(FILE *fp) {
	fseek(fp, 0, SEEK_SET);
	if (fgetc(fp) == 'P' && fgetc(fp) == '6') {
		return 1;
	}
	return 0;
}

static int read_to_wspace(FILE *fp, char *buf, int bsize) {
	int c, count = 0;

	while ((c = fgetc(fp)) != -1 && !isspace(c) && count < bsize - 1) {
		if (c == '#') {
			while ((c = fgetc(fp)) != -1 && c != '\n' && c != '\r');
			c = fgetc(fp);
			if (c == '\n' || c == '\r') continue;
		}
		*buf++ = c;
		count++;
	}
	*buf = 0;

	while ((c = fgetc(fp)) != -1 && isspace(c));
	ungetc(c, fp);
	return count;
}

void *load_ppm(FILE *fp, unsigned long *xsz, unsigned long *ysz) {
	char buf[64];
	int bytes, raw;
	unsigned int w, h, i, sz;
	uint32_t *pixels;

	fseek(fp, 0, SEEK_SET);

	bytes = read_to_wspace(fp, buf, 64);
	raw = buf[1] == '6';

	if ((bytes = read_to_wspace(fp, buf, 64)) == 0) {
		fclose(fp);
		return 0;
	}
	if (!isdigit(*buf)) {
		fprintf(fp, "load_ppm: invalid width: %s", buf);
		fclose(fp);
		return 0;
	}
	w = atoi(buf);

	if ((bytes = read_to_wspace(fp, buf, 64)) == 0) {
		fclose(fp);
		return 0;
	}
	if (!isdigit(*buf)) {
		fprintf(fp, "load_ppm: invalid height: %s", buf);
		fclose(fp);
		return 0;
	}
	h = atoi(buf);

	if ((bytes = read_to_wspace(fp, buf, 64)) == 0) {
		fclose(fp);
		return 0;
	}
	if (!isdigit(*buf) || atoi(buf) != 255) {
		fprintf(fp, "load_ppm: invalid or unsupported max value: %s", buf);
		fclose(fp);
		return 0;
	}

	if (!(pixels = malloc(w * h * sizeof *pixels))) {
		fputs("malloc failed", fp);
		fclose(fp);
		return 0;
	}

	sz = h * w;
	for (i = 0; i<sz; i++) {
		int r = fgetc(fp);
		int g = fgetc(fp);
		int b = fgetc(fp);

		if (r == -1 || g == -1 || b == -1) {
			free(pixels);
			fclose(fp);
			fputs("load_ppm: EOF while reading pixel data", fp);
			return 0;
		}
		pixels[i] = PACK_COLOR24(r, g, b);
	}

	fclose(fp);

	if (xsz) *xsz = w;
	if (ysz) *ysz = h;
	return pixels;
}
