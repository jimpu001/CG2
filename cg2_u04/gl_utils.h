#pragma once
#ifndef GL_UTILS_H
#define GL_UTILS_H
#include <string>
#include "glad/glad.h"
#include "util.h"

inline bool cg2_check_shader(GLuint shader)
{
	GLint is_compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);

	GLint max_length = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

	// The maxLength includes the NULL character
	std::string errorLog;
	errorLog.resize(static_cast<GLuint>(max_length));
	glGetShaderInfoLog(shader, max_length, &max_length, &errorLog[0]);


	if (is_compiled == GL_FALSE){
		warn("********** COMPILING SHADER FAILED **********");
		warn("\n%s\n", errorLog.c_str());
		return false;
	}else{
		if (max_length)
			info("GL Shader info:\n%s\n", errorLog.c_str());
	}
	return  true;
}

inline bool cg2_check_program(GLuint prog)
{
	GLint is_linked = 0;
	glGetProgramiv(prog, GL_LINK_STATUS, &is_linked);

	GLint max_len = 0;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &max_len);

	// The maxLength includes the NULL character
	std::string errorLog;
	errorLog.resize(static_cast<GLuint>(max_len));
	glGetProgramInfoLog(prog, max_len, &max_len, &errorLog[0]);


	if (is_linked == GL_FALSE){
		warn("********** LINKING PROGRAM FAILED **********");
		warn("\n%s\n", errorLog.c_str());
		return false;
	}else{
		if (max_len)
			info("GL Program info:\n%s\n", errorLog.c_str());

	}
	return  true;
}


inline GLuint cg2_load_and_compile_shader(GLenum type,const std::string& path)
{
	GLuint shader = glCreateShader(type);
	std::string code = load_file(path);

	const GLchar* code_ptr = code.c_str();
	glShaderSource(shader,
		       1, // how many elements code array
		       &code_ptr, // code array
		       nullptr // length of the strings in code array. nullptr -> zero terminated strings;
		       );
	glCompileShader(shader);
	if(!cg2_check_shader(shader)){
		glDeleteShader(shader);
		shader = 0;
	}
	return shader;
}


inline GLuint cg2_load_compile_link_program(const std::string& path_vs,
					    const std::string& path_fs)
{
	GLuint prog;
	// Load the shaders
	GLuint vs = cg2_load_and_compile_shader(GL_VERTEX_SHADER, path_vs);
	GLuint fs = cg2_load_and_compile_shader(GL_FRAGMENT_SHADER, path_fs);

	// Create the program, attach shaders and link the program
	prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);

	glLinkProgram(prog);
	cg2_check_program(prog);

	// delete the shaders
	glDeleteShader(vs);
	glDeleteShader(fs);

	return prog;
}


/* Check for GL errors. If ignore is not set, print a warning if an error was
 * encountered.
 * Returns GL_NO_ERROR if no errors were set. */
inline GLenum cg2_get_gl_error(const char* action,
                         bool ignore,
                         const char* file,
                         const int line,
                         std::string* msg)
{
	GLenum e,err=GL_NO_ERROR;

	do {
		e=glGetError();
		if ( (e != GL_NO_ERROR) && (!ignore) ) {
			err=e;
			if (msg) {
				if (file)
					*msg += std::string(file)+":";
				if (line)
					*msg += std::to_string(line)+":";
				*msg +="GL error "+std::to_string(err)+" at "+std::string(action);
			} else {
				if (file)
					fprintf(stderr,"%s:",file);
				if (line)
					fprintf(stderr,"%d:",line);
				warn("GL error %d at %s",err,action);
			}
		}
	} while (e != GL_NO_ERROR);
	return err;
}



// ############################################################################

/**
 * @brief The CG2GLWatch class is used to measure time on both client and GPU
 * side. With take() the time is taken, with get_... the time is returned.
 */
template <unsigned int NUM_BUFFS = 16>
class CG2GLWatch
{
	GLuint query[NUM_BUFFS];
	GLint64 cpu_time[NUM_BUFFS];
	GLuint next;

public:
	CG2GLWatch()
	{
		next = 0;
		glGenQueries(NUM_BUFFS, query);
		for (unsigned int i = 0; i<NUM_BUFFS;i++)
			cpu_time[i]=0;
	}
	~CG2GLWatch()
	{
		glDeleteQueries(NUM_BUFFS,query);
		for (unsigned int i = 0; i<NUM_BUFFS;i++)
			query[i] = 0;
	}

	void take()
	{

		glQueryCounter(query[next],GL_TIMESTAMP);
		glGetInteger64v(GL_TIMESTAMP,cpu_time+next);
		next = (next+1)%NUM_BUFFS;
	}

	/**
	 * @brief get_time_in_ms will query and return the oldest gpu timestamp!
	 * @return time in ms
	 */
	double get_gpu_time_in_ms()
	{
		GLuint64 time_ns =0;
		glGetQueryObjectui64v(query[(next)%NUM_BUFFS],GL_QUERY_RESULT,&time_ns);
		return static_cast<double>(time_ns)/1000000.0;
	}

	/**
	 * @brief get_time_in_ms will return the oldest cpu timestamp!
	 * @return time in ms
	 */
	double get_cpu_time_in_ms()
	{
		return static_cast<double>(cpu_time[next])/1000000.0;
	}
};



#endif // GL_UTILS_H
