/*
* Program.cpp
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

#include "Program.h"
#include "Shader.h"

#include <iostream>

#include <GL\glew.h>

Program::Program(Shader & vertex, Shader & fragment) : vert(vertex), frag(fragment) {
	if (!(vertex.getType() == GL_VERTEX_SHADER) || !(fragment.getType() == GL_FRAGMENT_SHADER)) {
		std::cout << "ERROR::PROGRAM::SHADERS_INCORRECT_TYPES" << std::endl;
	}

	this->id = NULL;
	this->vert = vertex;
	this->frag = fragment;
}

void Program::create(void) {
	GLint success;
	GLchar infoLog[512];

	this->id = glCreateProgram();
	glAttachShader(this->id, (this->vert).getId());
	glAttachShader(this->id, (this->frag).getId());
	glLinkProgram(this->id);

	glGetProgramiv(this->id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->id, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
}
GLuint Program::getId(void) {
	return this->id;
}