//
//    Many useful helper functions for GLSL shaders - gleaned from various sources including orange book
//    Created by zwood on 2/21/10.
//    Modified by sueda 10/15/15.
//

#pragma once
#ifndef __GLSL__
#define __GLSL__

#define GLEW_STATIC
#include <GL/glew.h>

///////////////////////////////////////////////////////////////////////////////
// For printing out the current file and line number                         //
///////////////////////////////////////////////////////////////////////////////
#include <sstream>

template <typename T>
std::string NumberToString(T x)
{
    std::ostringstream ss;
    ss << x;
    return ss.str();
}

#define GET_FILE_LINE (std::string(__FILE__) + ":" + NumberToString(__LINE__)).c_str()
///////////////////////////////////////////////////////////////////////////////

namespace GLSL {

    void checkError(const char *str = 0);
    void printProgramInfoLog(GLuint program);
    void printShaderInfoLog(GLuint shader);
    void checkVersion();
    bool textFileRead(const std::string & filepath, std::string & r_dst);
    int textFileWrite(const char *filename, char *s);
    GLuint createShader(const std::string &, GLenum type);
}

#endif