#include "Utility.h"

#include <cstdio>
#include "gl_core_4_4.h"


bool LoadShaderType(char* filename, GLenum shader_type, unsigned int* output)
{
	bool succeded = true;
	//int infoLogLength = 0;

	FILE* shader_file = fopen(filename, "r");

	if (shader_file == 0)
	{
		succeded = false;
	}
	else
	{
		//find file length
		fseek(shader_file, 0, SEEK_END);
		int shader_file_length = ftell(shader_file);
		fseek(shader_file, 0, SEEK_SET);

		//allocate space
		char *shader_source = new char[shader_file_length];

		shader_file_length = fread(shader_source, 1, shader_file_length, shader_file);

		unsigned int shader_handle = glCreateShader(shader_type);

		glShaderSource(shader_handle, 1, &shader_source, &shader_file_length);
		glCompileShader(shader_handle);

		int success = GL_FALSE;
		glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			int infoLogLength = 0;
			glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &infoLogLength);
			char* log = new char[infoLogLength];
			glGetShaderInfoLog(shader_handle, infoLogLength, 0, log);
			printf("ERROR: Failed\n\n");
			printf("%s", log);
			delete[] log;
			succeded = false;
		}

		if (succeded)
		{
			*output = shader_handle;
		}

		delete[] shader_source;
		fclose(shader_file);
	}
	return succeded;

}



bool LoadShaders(char* a_vertex_filename, char* a_fragment_filename, char* a_geometry_filename, GLuint* a_result)
{
	*a_result = glCreateProgram();

	bool succeeded = true;
	unsigned int vertex_shader;
	unsigned int geometry_shader;
	unsigned int fragment_shader;


	if (LoadShaderType(a_vertex_filename, GL_VERTEX_SHADER, &vertex_shader))
	{
		glAttachShader(*a_result, vertex_shader);
		glDeleteShader(vertex_shader);
	}
	else
		printf("Vertex shader failed to load");

	if (a_geometry_filename != nullptr)
	{
		if (LoadShaderType(a_geometry_filename, GL_GEOMETRY_SHADER, &geometry_shader))
		{

			glAttachShader(*a_result, geometry_shader);
			glDeleteShader(geometry_shader);
		}
		else
			printf("Geometry shader failed to load");
	}

	if (a_fragment_filename != nullptr)
	{
		if (LoadShaderType(a_fragment_filename, GL_FRAGMENT_SHADER, &fragment_shader))
		{
			glAttachShader(*a_result, fragment_shader);
			glDeleteShader(fragment_shader);
		}
		else
			printf("Fragment shader failed to load");
	}

	
	glLinkProgram(*a_result);

	//error checking
	GLint success = GL_FALSE;
	glGetProgramiv(*a_result, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(*a_result, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(*a_result, infoLogLength, 0, infoLog);
		printf("ERROR: Failed to link program\n\n");
		printf("%s", infoLog);
		delete[] infoLog;
		succeeded = false;
	}

	
	return succeeded;

}