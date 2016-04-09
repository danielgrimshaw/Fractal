/*
* Shader.cpp
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

#include <GL\glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Shader.h"

Shader::Shader(GLenum type, std::string filename) {
	std::ifstream shaderFile;

	this->shaderType = type;

	shaderFile.exceptions(std::ifstream::badbit);

	try {
		// Open files
		shaderFile.open(filename);
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();

		this->code = shaderStream.str();
	} catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_READ_SUCCESSFULLY" << std::endl;
		this->code = "";
	}
	

	this->id = NULL;
}

void Shader::create(void) {
	GLint success;
	GLchar infoLog[512];
	const GLchar * shaderCode = code.c_str();
	this->id = glCreateShader(this->shaderType);//ILLEGAL MEMORY ACCESS
	glShaderSource(this->id, 1, &shaderCode, NULL);
	glCompileShader(this->id);

	glGetShaderiv(this->id, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(this->id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

GLuint Shader::getId(void) {
	return this->id;
}

GLenum Shader::getType(void) {
	return this->shaderType;
}