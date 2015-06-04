#ifndef _UTIL_H_
#define _UTIL_H_
#include <iostream>
#include <cstdlib>

unsigned long get_msec(void);

void *load_image(const char *fname, unsigned long *xsz, unsigned long *ysz);

typedef struct options {
	unsigned int window, width, height, vertex, fragment;
	char * vpath, fpath;
	const char * define = "dE";
} Options;

class Shader {
public:
	void setDefaultsOpts();
	void setOpts(void * opts[]);
	void ** optionsAsArray();
	void setParameter();
	void load(const char * vname, const char * fname);
	bool createShader(const char * vertex_src, const char * fragment_src);
	const char * parseShader(const char * type, const char * src);
	unsigned int createShader(const char * type, const char * src, const char * opts);
private:
	Options opt;
	void loadShader(char * type, char * path) {
		using namespace std;
		unsigned int sdr;
		char * src_buf;
		int success, linked;
		string str;
		ifstream t;

		t.open(path);
		t.seekg(0, std::ios::end);
		str.reserve(t.tellg());
		t.seekg(0, std::ios::beg);

		str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

		src_buf = new char[str.length() + 1];
		src_buf = (char *)str.c_str();
		src_buf[str.length()] = 0;
		t.close();

		if (type == "vertex") {
			sdr = glCreateShader(GL_VERTEX_SHADER);
		}
		else if (type == "fragment") {
			sdr = glCreateShader(GL_FRAGMENT_SHADER);
		}
		else {
			cout << "Error! Invalid shader type" << endl;
			exit(-1);
		}

		glShaderSource(sdr, 1, (const char **)&src_buf, 0);
		glCompileShader(sdr);
		glGetShaderiv(sdr, GL_COMPILE_STATUS, &success);
		if (!success) {
			int info_len;
			char *info_log;

			glGetShaderiv(sdr, GL_INFO_LOG_LENGTH, &info_len);
			if (info_len > 0) {
				if (!(info_log = new char[info_len + 1])) {
					cout << "Unable to allocate info_log (util.cpp: line 90)" << endl;
					exit(-2);
				}
				glGetShaderInfoLog(sdr, info_len, 0, info_log);
				cout << type << " shader compilation failed: " << info_log << endl;
				delete[] info_log;
			}
			else {
				cout << type <<" shader compilation failed" << endl;
			}
			exit(-1);
		}
		if (type == "vertex") {
			opt.vertex = sdr;
		}
		else if (type == "fragment") {
			opt.fragment = sdr;
		}
		else {
			cout << "Error! Invalid shader type" << endl;
			exit(-1);
		}
	}
};

void reset();
void resize(unsigned int width, unsigned int height);
unsigned int * getPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

void sendUniforms();
void sendUniform(int key);

void loadTextures();
void loadTexture(const char * path);
#endif	/* _UTIL_H_ */
