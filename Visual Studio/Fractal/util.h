#ifndef __UTIL_H__
#define __UTIL_H__
#include <iostream>
#include <cstdlib>
#include <fstream>

unsigned long get_msec(void);

void *load_image(const char *fname, unsigned long *xsz, unsigned long *ysz);

class Shader {
public:
	void setDefaultUniforms();
	void set_uniform1f(const char *name, float val);
	void set_uniform2f(const char *name, float v1, float v2);
	void set_uniform1i(const char *name, int val);
	void setParameter();
	void load(const char * vname, const char * fname);
	const char * parseShader(const char * type, const char * src);
	unsigned int createShader(const char * type, const char * src, const char * opts);
private:
	unsigned int program;
	unsigned int loadShader(char * type, char * path) {
		using namespace std;
		unsigned int sdr;
		char * src_buf;
		int success;
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
		return sdr;
	}
};

void reset();
void resize(unsigned int width, unsigned int height);
unsigned int * getPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

void loadTextures();
void loadTexture(const char * path);

PCTSTR getLeftStrings();
PCTSTR getRightStrings();
PCTSTR getControls();
#endif	/* _UTIL_H_ */
