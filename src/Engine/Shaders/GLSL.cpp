//
//    Many useful helper functions for GLSL shaders - gleaned from various sources including orange book
//    Created by zwood on 2/21/10.
//    Modified by sueda 10/15/15.
//

#include "GLSL.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <cassert>
#include <fstream>

namespace GLSL {

const char * errorString(GLenum err)
{
    switch(err) {
    case GL_NO_ERROR:
        return "No error";
    case GL_INVALID_ENUM:
        return "Invalid enum";
    case GL_INVALID_VALUE:
        return "Invalid value";
    case GL_INVALID_OPERATION:
        return "Invalid operation";
    case GL_STACK_OVERFLOW:
        return "Stack overflow";
    case GL_STACK_UNDERFLOW:
        return "Stack underflow";
    case GL_OUT_OF_MEMORY:
        return "Out of memory";
    default:
        return "No error";
    }
}

void checkError(const char *str)
{
    GLenum glErr = glGetError();
    if(glErr != GL_NO_ERROR) {
        if(str) {
            printf("%s: ", str);
        }
        printf("GL_ERROR = %s.\n", errorString(glErr));
        assert(false);
    }
}

void printShaderInfoLog(GLuint shader)
{
    GLint infologLength = 0;
    GLint charsWritten  = 0;
    GLchar *infoLog;
    
    checkError(GET_FILE_LINE);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
    checkError(GET_FILE_LINE);
    
    if(infologLength > 0) {
        infoLog = (GLchar *)malloc(infologLength);
        if(infoLog == NULL) {
            puts("ERROR: Could not allocate InfoLog buffer");
            exit(1);
        }
        glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
        printf("Shader InfoLog:\n%s\n\n", infoLog);
        free(infoLog);
    }
    checkError(GET_FILE_LINE);
}

void printProgramInfoLog(GLuint program)
{
    GLint infologLength = 0;
    GLint charsWritten  = 0;
    GLchar *infoLog;
    
    checkError(GET_FILE_LINE);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
    checkError(GET_FILE_LINE);
    
    if(infologLength > 0) {
        infoLog = (GLchar *)malloc(infologLength);
        if(infoLog == NULL) {
            puts("ERROR: Could not allocate InfoLog buffer");
            exit(1);
        }
        glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
        printf("Program InfoLog:\n%s\n\n", infoLog);
        free(infoLog);
    }
    checkError(GET_FILE_LINE);
}

void checkVersion()
{
    int major, minor;
    major = minor = 0;
    const char *verstr = (const char *)glGetString(GL_VERSION);
    
    if((verstr == NULL) || (sscanf(verstr, "%d.%d", &major, &minor) != 2)) {
        printf("Invalid GL_VERSION format %d.%d\n", major, minor);
    }
    if(major < 2) {
        printf("This shader example will not work due to the installed Opengl version, which is %d.%d.\n", major, minor);
        exit(0);
    }
}

GLuint createShader(const std::string & filepath, GLenum type) {
    GLint rc;
    GLuint shaderId = glCreateShader(type);

    std::string shader;
    if (!GLSL::textFileRead(filepath, shader)) {
        std::cerr << "Could not read shader file: " << filepath << std::endl;
        return 0;
    }
    const char * src(shader.c_str());
    glShaderSource(shaderId, 1, &src, NULL);

    // Compile shader
    glCompileShader(shaderId);
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &rc);
    if (!rc) {
        GLSL::printShaderInfoLog(shaderId);
        std::cerr << "Error compiling shader: " << filepath << std::endl;
        return 0;
    }

    return shaderId;
}

bool textFileRead(const std::string & filepath, std::string & r_dst) {
    std::ifstream ifs(filepath);
    if (!ifs.good()) {
        return false;
    }

    std::stringstream ss;
    ss << ifs.rdbuf();
    ifs.close();
    r_dst = std::move(ss.str());

    return true;
}

int textFileWrite(const char *fn, char *s)
{
    FILE *fp;
    int status = 0;
    if(fn != NULL) {
        fp = fopen(fn,"w");
        if(fp != NULL) {
            if(fwrite(s,sizeof(char),strlen(s),fp) == strlen(s)) {
                status = 1;
            }
            fclose(fp);
        }
    }
    return(status);
}
}