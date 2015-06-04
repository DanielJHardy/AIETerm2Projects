#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "glm_header.h"
#include "gl_core_4_4.h"

typedef unsigned int GLuint;
bool LoadShaders(char* a_vertex_filename, char* a_fragment_filename, char* a_geometry_filename, GLuint* a_result);
bool LoadShaderType(char* filename, GLenum shader_type, unsigned int* output);


#endif // !_UTILITY_H_
