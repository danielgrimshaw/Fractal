/*
* Program.h
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

#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include "Shader.h"
class Program {
public:
	Program(Shader & vertex, Shader & fragment);
	void create(void);
	GLuint getId(void);
private:
	GLuint id;
	Shader & vert;
	Shader & frag;
};
#endif