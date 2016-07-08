#include "SimpleGLShader.h"
#include <fstream>
#include <iostream>

SimpleGLShader::SimpleGLShader(void) : mTotalShaders(0) {
    mProgram = -1;
    mAttrsList.clear();
    mUniformList.clear();
}

SimpleGLShader::~SimpleGLShader(void) {
    destroy();
}


void SimpleGLShader::loadFromText(const std::string& vsSource, const std::string& fsSource) {
	loadFromText(vsSource, GL_VERTEX_SHADER);
	loadFromText(fsSource, GL_FRAGMENT_SHADER);
}

void SimpleGLShader::loadFromText(const std::string& source, GLenum type) {
	// Create and compile shader
    GLuint shader;
    shader = glCreateShader(type);
	const char *c_str = source.c_str();
	glShaderSource(shader, 1, &c_str, NULL);

    GLint status;
    glCompileShader(shader);
    glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;        
        glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar *infoLog= new GLchar[infoLogLength];
        glGetShaderInfoLog (shader, infoLogLength, NULL, infoLog);
        std::cerr << "Compile log: " << infoLog << std::endl;
        delete [] infoLog;
        return;
    }

    // Add to shaders in use
    mShaders.push_back(shader);
}

void SimpleGLShader::load(const std::string& fileName, GLenum type) {
    unsigned int fileLen;

    // Load file
    std::ifstream file;
    file.open(fileName.c_str(), std::ios::in);
    if (!file) {
        std::cout << "File " << fileName << " not found" << std::endl;
        exit(-1);
    }

    // File length
    file.seekg(0, std::ios::end);
    fileLen = int(file.tellg());
    file.seekg(std::ios::beg);

    // Read the file
    char *source = new char[fileLen + 1];

    int i = 0;
    while (file.good()) {
        source[i] = char(file.get());
        if (!file.eof()) i++;
        else fileLen = i;
    }
    source[fileLen] = '\0';
    file.close();

    // Create and compile shader
    GLuint shader;
	GLuint vertex_shader = GL_VERTEX_SHADER;
	GLuint frag_shader = GL_FRAGMENT_SHADER;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar **) &source, (const GLint *) &fileLen);

    GLint status;
    glCompileShader(shader);
    glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;        
        glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar *infoLog= new GLchar[infoLogLength];
        glGetShaderInfoLog (shader, infoLogLength, NULL, infoLog);
        std::cerr << "Compile log ("<< fileName << "): " << infoLog << std::endl;
        delete [] infoLog;
        return;
    }
    delete source;

    // Add to shaders in use
    mShaders.push_back(shader);
}

void SimpleGLShader::load(const std::string& vsFile, const std::string& fsFile) {
    load(vsFile, GL_VERTEX_SHADER);
    load(fsFile, GL_FRAGMENT_SHADER);
}

void SimpleGLShader::destroy() {
    mAttrsList.clear();
    mUniformList.clear();
    GLuint size = mShaders.size();
    for(GLuint i = 0; i < size; i++) {
        if(mShaders[i] != 0) {
            glDetachShader(mProgram, mShaders[i]);
        }
    }
    glDeleteProgram(mProgram);
}

void SimpleGLShader::create() {
    mProgram = glCreateProgram();
    GLuint size = mShaders.size();
    for(GLuint i = 0; i < size; i++) {
        if(mShaders[i] != 0) {
            glAttachShader(mProgram, mShaders[i]);
        }
    }
}

void SimpleGLShader::link() {
    // link and check whether the program links fine
    GLint status;
    glLinkProgram (mProgram);
    glGetProgramiv (mProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        
        glGetProgramiv (mProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar *infoLog = new GLchar[infoLogLength];
        glGetProgramInfoLog (mProgram, infoLogLength, NULL, infoLog);
        std::cerr << "Link log: " << infoLog << std::endl;
        delete [] infoLog;
    }
}

void SimpleGLShader::use() {
    glUseProgram(mProgram);
}

void SimpleGLShader::unuse() {
    glUseProgram(-1);
}

void SimpleGLShader::compile_and_link() {
	create();
	link();
} 
 
GLuint SimpleGLShader::program() { 
    return mProgram; 
} 

void SimpleGLShader::add_attribute(const std::string& attr) {
    mAttrsList[attr] = glGetAttribLocation(mProgram, attr.c_str());
}

void SimpleGLShader::add_uniform(const std::string& uniform_name) {
    mUniformList[uniform_name] = glGetUniformLocation(mProgram, uniform_name.c_str());
}

void SimpleGLShader::bind_attribute(const std::string& attr, GLuint index) {
    glBindAttribLocation(mProgram, index, attr.c_str());
}

GLuint SimpleGLShader::attribute(const std::string& attr) {
    return mAttrsList[attr];
}

GLuint SimpleGLShader::uniform(const std::string& uniform_name) {
    return mUniformList[uniform_name];
}

void SimpleGLShader::send_uniform(const std::string& uniform_name, float x, float y, float z) {
	GLint loc = uniform(uniform_name);
	glUniform3f(loc, x, y, z);
}

void SimpleGLShader::send_uniform(const std::string& uniform_name, const vec2 & v) {
    GLint loc = uniform(uniform_name);
	glUniform2f(loc, v.x, v.y);
}

void SimpleGLShader::send_uniform(const std::string& uniform_name, const vec3 & v) {
    this->send_uniform(uniform_name, v.x, v.y, v.z);
}

void SimpleGLShader::send_uniform(const std::string& uniform_name, const vec4 & v) {
    GLint loc = uniform(uniform_name);
    glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void SimpleGLShader::send_uniform(const std::string& uniform_name, const mat4 & m, GLboolean inverse) {
    GLint loc = uniform(uniform_name);
    glUniformMatrix4fv(loc, 1, inverse, &m[0][0]);
}

void SimpleGLShader::send_uniform(const std::string& uniform_name, const mat3 & m) {
    GLint loc = uniform(uniform_name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

void SimpleGLShader::send_uniform(const std::string& uniform_name, GLfloat val) {
    GLint loc = uniform(uniform_name);
    glUniform1f(loc, val);
}

void SimpleGLShader::send_uniform(const std::string& uniform_name, int val) {
    GLint loc = uniform(uniform_name);
    glUniform1i(loc, val);
}

void SimpleGLShader::send_uniform_b(const std::string& uniform_name, bool val) {
    GLint loc = uniform(uniform_name);
    glUniform1i(loc, val);
}

void SimpleGLShader::send_uniform(const std::string& uniform_name, GLuint val) {
    GLint loc = uniform(uniform_name);
    glUniform1ui(loc, val);
}