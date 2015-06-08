#ifndef __FRACTAL_H__
#define __FRACTAL_H__ // Don't include this file multiple times.
void startFractal(); // Launches the Fractal Window
void draw(void); // Handler for redrawing
void idle_handler(void); // Handler for when nothing is happenning
void key_handler(unsigned char key, int x, int y); // keyboard event handler
                                           // This controls everything for now
void bn_handler(int bn, int state, int x, int y); // Mouse handler
void mouse_handler(int x, int y); // Mouse movement handler
#endif