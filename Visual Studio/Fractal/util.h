#ifndef _UTIL_H_
#define _UTIL_H_

unsigned long get_msec(void);

void *load_image(const char *fname, unsigned long *xsz, unsigned long *ysz);

typedef struct options {
	unsigned int window, width, height;
	char vertex[], fragment[], vpath[], fpath[];
	const char * define = "dE";
} Options;

class Shader {
public:
	void setDefaultsOpts();
	void setParameter();
	void loadShaders(const char * vname, const char * fname);
	void loadShader(char * type, char * path);
	bool createShader(const char * vertex_src, const char * fragment_src);
	const char * parseShader(const char * type, const char * src);
	unsigned int createShader(const char * type, const char * src, const char * opts);
private:
	Options opt;
};

void reset();
void resize(unsigned int width, unsigned int height);
unsigned int * getPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

void sendUniforms();
void sendUniform(int key);

void loadTextures();
void loadTexture(const char * path);
#endif	/* _UTIL_H_ */
