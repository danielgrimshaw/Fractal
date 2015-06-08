#ifndef __UTIL_H__
#define __UTIL_H__
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstdint>
#include <cmath>
unsigned long get_msec(void);

// Constants for 2D fractal types
#define MANDELBROT 0
#define ORBITTRAP 1
#define DUCKS 2

class Shader {
public:
	// Set uniforms (naming scheme due to openGL standards)
	void set_uniform1f(const char * name, float val);
	void set_uniform2f(const char * name, float v1, float v2);
	void set_uniform3f(const char * name, float v1, float v2, float v3);
	void set_uniform1i(const char * name, int val);
	void updateValueStrings(); // Update the uniform printout strings
	void load(const char * vname, const char * fname); // load shaders
	void use();
	GLint getAttribLocation(const char * name);
	void Shader::toggle(const char * name);
private:
	unsigned int program; // Shader program
	unsigned int loadShader(const char * type, const char * path) { // load a specific shader
		using namespace std;
		unsigned int sdr; // Shader id
		char * src_buf; // Shader source code
		int success;
		string str; // Temporary holder for shader source
		ifstream t; // File handler

		t.open(path); // Open the shader
		t.seekg(0, std::ios::end); // Go to the end
		str.reserve(t.tellg()); // read location (this is the size) and set string to that size
		t.seekg(0, std::ios::beg); // Go back to the begining

		// Set the string to the file data (DO NOT BREAK THIS)
		// Automatically iterates through the entire stream of characters before
		// compiling them into a character array to form a string.
		str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

		src_buf = new char[str.length() + 1]; // allocate space for c-style string
		src_buf = (char *)str.c_str(); // create c style string
		src_buf[str.length()] = 0; // Code doesn't work with out this, not sure why not though
		t.close(); // close file buffer

		if (std::string("vertex").compare(type) == 0) { // This is a vertex shader
			sdr = glCreateShader(GL_VERTEX_SHADER);
		}
		else if (std::string("fragment").compare(type) == 0) { // This is a fragment shader
			sdr = glCreateShader(GL_FRAGMENT_SHADER);
		}
		else { // This is a very unusual error
			cout << "Error! Invalid shader type" << endl;
			exit(-1);
		}

		/* At this point src_buf really should be deleted.
		 * it is just taking up extra space.
		 * Unfortunately, doing that destroys str.
		 * This causes a quiet error in the code and led to about
		 * 6 hours of wasted time trying to track this bug.
		 * Leaving the memory allocated will take up about a
		 * megabyte more ram, but allows the program to run.
		 */

		glShaderSource(sdr, 1, (const char **)&src_buf, 0); // Tell OGL where the source is
		glCompileShader(sdr); // compile the shader source
		glGetShaderiv(sdr, GL_COMPILE_STATUS, &success); // Did it work?
		if (!success) {
			int info_len; // No, it did not work
			char *info_log;

			glGetShaderiv(sdr, GL_INFO_LOG_LENGTH, &info_len); // how long is the error
			if (info_len > 0) {
				if (!(info_log = new char[info_len + 1])) { // Need to buy more ram
					cout << "Unable to allocate info_log (util.cpp: line 90)" << endl;
					exit(-2);
				}
				glGetShaderInfoLog(sdr, info_len, 0, info_log); // Write the log
				cout << type << " shader compilation failed: " << info_log << endl;
				delete[] info_log; // this works here
			}
			else {
				cout << type << " shader compilation failed" << endl; // There is no info log!
			}
			exit(-1); // User should never see a broken program.
		}
		return sdr; // Return the shader ID
	}
};

void setDefaultUniforms2d(Shader * shaders); // Set up the 2d shaders
void setDefaultUniforms3d(Shader * shaders); // Set up the 3d shaders (EXPERIMENTAL)
void resize(unsigned int width, unsigned int height); // Resize window
uint8_t * getPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height); // Get all the pixels

#if !defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)
#if  defined(__i386__) || defined(__ia64__) || defined(WIN32) || \
    (defined(__alpha__) || defined(__alpha)) || \
     defined(__arm__) || \
    (defined(__mips__) && defined(__MIPSEL__)) || \
     defined(__SYMBIAN32__) || \
     defined(__x86_64__) || \
     defined(__LITTLE_ENDIAN__)
// little endian (On my system, everything is defined!)
#define LITTLE_ENDIAN
#else
// big endian
#define BIG_ENDIAN
#endif	// endian check
#endif	// !defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)

#ifdef LITTLE_ENDIAN // This almost isn't even worth making compatibility for
#define PACK_COLOR24(r, g, b) (((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff))
#else
#define PACK_COLOR24(r, g, b) (((b & 0xff) << 16) | ((g & 0xff) << 8) | (r & 0xff))
#endif

class Texture {
public:
	void load(const char * path); // load specific texture
	void enable();
private:
	GLuint texture;
	void * load_image(const char *fname, unsigned long *xsz, unsigned long *ysz) {
		using namespace std;
		ifstream fp(fname); // Open fname for reading
		if (!(fp.is_open())) { // Can't open fname
			cout << "failed to open: " << fname << endl;
			return 0;
		}

		if (check_ppm(fp)) { // Is it actually a P6 ppm image file?
			return load_ppm(fp, xsz, ysz); // load the image
		}

		fp.close(); // Close buffer
		cout << "unsupported image format" << endl; // It didn't work
		return 0; // classic C-style boolean
	}

	int check_ppm(std::ifstream & fp) {
		using namespace std;
		fp.seekg(0, ios::beg); // Go to the begining of the file again
		if (fp.get() == 'P' && fp.get() == '6') { // is it P6?
			return 1; // It worked (1 == true)
		}
		return 0; // Not a P6 ppm (0 == false)
	}

	int read_to_wspace(std::ifstream & fp, char * buf, int bsize) { // read until whitespace
		int count = 0;
		char c;

		while (fp.get(c) && !isspace(c) && count < bsize - 1) { // While not whitespace nor max size
			if (c == '#') { // if '#'
				while (fp.get(c) && c != '\n' && c != '\r'); // While not a newline or EOF, keep reading
				c = fp.get(); // What is the character now?
				if (c == '\n' || c == '\r') continue; // not the end of the file, keep going
			}
			*buf++ = c; // next element in buf is now c
			count++;
		}
		*buf = 0;

		while (fp.get(c) && isspace(c)); // while spaces, read
		fp.putback(c); // return one space to the stream
		return count;
	}

	void * load_ppm(std::ifstream & fp, unsigned long *xsz, unsigned long *ysz) { // load a ppm P6 image file
		using namespace std;
		char buf[64];
		int bytes, raw;
		unsigned int w, h, i, sz;
		uint32_t *pixels;

		fp.seekg(0, ios::beg); // Go to the begining of the file

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
				delete[] pixels;
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

};

class Camera {
public:
	float x, y, z;
	Camera(Shader * shaders, float xval, float yval, float zval, float pitch, float yaw) {
		x = xval; y = yval; z = zval;
		_pitch = pitch;
		_yaw = yaw;
		_step = 0.1f;
		shader = shaders;
	}

	float step() {
		return _step;
	}

	float pitch() {
		return _pitch;
	}

	float yaw() {
		return _yaw;
	}

	void forward(float dist) {
		move(dist, _yaw, _pitch);
	}

	void back(float dist) {
		move(-dist, _yaw, _pitch);
	}

	void up(float dist) {
		move(dist, _yaw, 90);
	}

	void down(float dist) {
		move(dist, _yaw, -90);
	}

	void strafeLeft(float dist) {
		move(dist, _yaw - 90, 0);
	}

	void strafeRight(float dist) {
		move(dist, _yaw + 90, 0);
	}

	void move(float s, float ay, float ap) {
		float hstep = cos(ap * deg2rad);

		x += hstep * sin(ay * deg2rad);
		z += hstep * cos(ay * deg2rad);
		y += sin(ap * deg2rad);
		shader->set_uniform3f("cameraPosition", x, y, z);
	}
private:
	float _pitch, _yaw, _step;
	const float deg2rad = 3.141593f / 180.0f;
	Shader * shader;
};
PCTSTR getLeftStrings(); // get left side uniform strings
PCTSTR getRightStrings(); // get right side uniform strings
PCTSTR getControls(); // get the list of controls
#endif	/* _UTIL_H_ */
