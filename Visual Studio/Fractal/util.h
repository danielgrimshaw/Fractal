#ifndef _UTIL_H_
#define _UTIL_H_

unsigned long get_msec(void);

void *load_image(const char *fname, unsigned long *xsz, unsigned long *ysz);

void setDefaultsOpts();
void setParameter();
void loadShaders(const char * vname, const char * fname);
void loadShader(char * type, char * path);
bool createShader(const char * vertex_src, const char * fragment_src);
const char * parseShader(const char * type, const char * src);
unsigned int createShader(const char * type, const char * src, const char * opts);
//void set_uniform1f(unsigned int prog, const char *name, float val);
//void set_uniform2f(unsigned int prog, const char *name, float v1, float v2);
//void set_uniform1i(unsigned int prog, const char *name, int val);

void reset();
void resize(unsigned int width, unsigned int height);
unsigned int * getPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

void sendUniforms();
void sendUniform(int key);

void loadTextures();
void loadTexture(const char * path);
#endif	/* _UTIL_H_ */