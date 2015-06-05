#ifndef __FRACTAL_H__
#define __FRACTAL_H__
int startFractal(int argc, char ** argv);
void draw(void);
void idle_handler(void);
void key_handler(unsigned char key, int x, int y);
void bn_handler(int bn, int state, int x, int y);
void mouse_handler(int x, int y);
#endif