// Simplify GLSL C style interaction with C++ 
// Copyright (C) <2015-2016> - Cristian Rodríguez Bernal (maldicion069) 
 
// This program is free software: you can redistribute it and/or modify 
// it under the terms of the GNU General Public License as published by 
// the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version. 
//  
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
// GNU General Public License for more details. 
//  
// You should have received a copy of the GNU General Public License 
// along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 
#ifndef _SIMPLEGLSHADER_H_ 
#define _SIMPLEGLSHADER_H_ 

#include <map> 
#include <string> 
#include <vector> 

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace glm;
 
class SimpleGLShader { 
public: 
    SimpleGLShader(void); 
    ~SimpleGLShader(void); 
 
    void destroy(); 
     
    void load(const std::string& vsFile, const std::string& fsFile); 
    void load(const std::string& file, GLenum type); 
	void loadFromText(const std::string& vsSource, const std::string& fsSource); 
	void loadFromText(const std::string& source, GLenum type);  

	void compile_and_link();
    GLuint program();

    void create(); 
    void link(); 
    void use(); 
    void unuse();
 
    void add_attribute(const std::string& attr); 
    void bind_attribute(const std::string& attr, GLuint index); 
    void add_uniform(const std::string& uniform); 
 
    GLuint attribute(const std::string& attr); 
    GLuint uniform(const std::string& uniform); 
 
    void send_uniform(const std::string& uniform, float x, float y, float z); 
    void send_uniform(const std::string& uniform, const vec2 & v); 
    void send_uniform(const std::string& uniform, const vec3 & v); 
    void send_uniform(const std::string& uniform, const vec4 & v); 
	void send_uniform(const std::string& uniform, const mat4 & m, GLboolean inverse = GL_FALSE); 
    void send_uniform(const std::string& uniform, const mat3 & m);
    void send_uniform(const std::string& uniform, GLfloat val); 
    void send_uniform(const std::string& uniform, int val); 
    void send_uniform_b(const std::string& uniform, bool val); 
    void send_uniform(const std::string& uniform, GLuint val); 
public: 
    GLuint mProgram; 
    int mTotalShaders; 
    std::map<std::string, GLuint> mAttrsList; 
    std::map<std::string, GLuint> mUniformList; 
    enum ShaderType {V_SHADER, F_SHADER}; 
    std::vector<GLuint> mShaders; 
}; 
 
#endif /* _SIMPLEGLSHADER_H_ */ 