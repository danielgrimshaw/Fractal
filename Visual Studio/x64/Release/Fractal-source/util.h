#ifndef _UTIL_H_
#define _UTIL_H_

unsigned long get_msec(void); // Get system runtime

void *load_image(const char *fname, unsigned long *xsz, unsigned long *ysz); // load ppm P6 image file

unsigned int setup_shader(const char *fname); // Sets up a shader
void set_uniform1f(unsigned int prog, const char *name, float val); // Set float uniforms
void set_uniform2f(unsigned int prog, const char *name, float v1, float v2); // Set vec2 uniforms
void set_uniform1i(unsigned int prog, const char *name, int val); // Set int and bool uniforms

#endif	// _UTIL_H_ 