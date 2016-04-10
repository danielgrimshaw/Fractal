/*
 * util.h
 * 
 * Copyright 2015  <Daniel Grimshaw>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

unsigned long get_msec(void); // Get system runtime

void * load_image(const char * fname, unsigned long * xsz, unsigned long * ysz); // load ppm P6 image file
char * read_file(const char * fname);

unsigned int setup_shader(const char * vertex_fname,
		const char * fragment_fname); // Sets up a shader
int compile_vertex(const char * vertex_fname);
int compile_fragment(const char * fragment_fname);

void set_uniform1f(unsigned int prog, const char * name, float val); // Set float uniforms
void set_uniform2f(unsigned int prog, const char * name, float v1, float v2); // Set vec2 uniforms
void set_uniform1i(unsigned int prog, const char * name, int val); // Set int and bool uniforms

#endif	// SRC_UTIL_H_
