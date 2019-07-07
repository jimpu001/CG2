#pragma once

#include "glad/glad.h"
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include "util.h"


typedef std::set<GLuint> ProgramIdentifier;
enum CG2BaseVtxShader
{
	BVTX_FULL = 0,
	BVTX_POSITION_ONLY,
	// always add new elements before the _COUNT
	BVTX_COUNT
};

enum CG2BaseFrgShader
{

	BFRG_NULL = 0,
	BFRG_WRITE_COLORBUFF,
	BFRG_GHOST,
	BFRG_SSPREPASS,
	// always add new elements before the _COUNT
	BFRG_COUNT
};


class CG2ShaderProgram
{
	friend class CG2ShaderCompositor;
	GLuint m_programs[BFRG_COUNT*BVTX_COUNT];
	bool link_bases;
public:
	CG2ShaderProgram(const CG2ShaderProgram& o)
	{
		for(int i = 0 ; i <BFRG_COUNT*BVTX_COUNT;i++){
			m_programs[i] = o.m_programs[i];
		}
	}

	CG2ShaderProgram()
	{
		for(int i = 0 ; i <BFRG_COUNT*BVTX_COUNT;i++){
			m_programs[i] = 0;
		}
	}

	void use(const CG2BaseVtxShader vtx = BVTX_FULL,
	         const CG2BaseFrgShader frg = BFRG_WRITE_COLORBUFF) const
	{
		glUseProgram(m_programs[vtx+BVTX_COUNT*frg]);
	}
};

struct CG2ShaderError
{
	std::string identifier;
	std::string message;

	void reset() {
		identifier.clear();
		message.clear();
	}
	operator bool() const {return !(message.empty()||identifier.empty());}
};

class CG2ShaderCompositor
{

	std::map<std::string,GLuint> m_shaders; // shaders by path
	std::map<GLuint,std::string> m_shader_paths; // path by shaders

	std::map<GLuint,CG2ShaderProgram*> m_raw_to_program; // raw glNames to programs
	std::map<GLuint,ProgramIdentifier> m_raw_to_program_id;
	std::map<ProgramIdentifier, CG2ShaderProgram*> m_programs; // program identifyer to program

	// a set of all raw programs ... handy to destroy them at the end or during reloading
	std::set<GLuint> m_raw_programs;

	// used for the compiler error output
	CG2ShaderError first_failed_shader;
	CG2ShaderError first_failed_program;

	// base shaders
	GLuint m_base_vtx_shader[BVTX_COUNT];
	GLuint m_base_frg_shader[BFRG_COUNT];

	// loads and compiles the sourcecode from path. If shader is set the
	// shader object will be used, if it is not set a new one will be created
	GLuint load_and_compile(const std::string& path, GLuint shader=0);

	// really link the OpenGL program object
	// assumes all shaders are already attached
	bool link_program(GLuint prog);

	// returns just the filename of a shader for debug outputs
	std::string get_shader_name(GLuint shader) const;

	// will create a program by linking every shader in pi. If link_bases is
	// set all permutations of base shaders will be linked to pi.
	// The results are stored in the returned CG2ShaderProgram
	CG2ShaderProgram* link_program(const ProgramIdentifier& pi,
	                               bool link_bases = true);

	// gives you the shader to a given path. If it does not exist
	// load_and_compile is called to create it
	GLuint get_shader(const std::string& path);

	// gives you the CG2ShaderProgram assoziated with pi, if it does not exist
	// link_program is called to create it.
	CG2ShaderProgram* get_program(const ProgramIdentifier& pi,
	                              bool link_base = true);

	// returns the program name for a specific program variant
	std::string get_program_name(const GLuint prog=0);

public:
	CG2ShaderCompositor();
	~CG2ShaderCompositor();

	// This will create the base shaders.
	void init();

	// This will destroy all OpenGL related date structures
	// including all shaders and programs
	void clear();

	// This will give you a ShaderProgram Object of the linked files as well as
	// if link_base is set all base shader permutations will be linked as well.
	// The ShaderCompositor takes care of this program object, so you don't have
	// to worry about deleting it.
	CG2ShaderProgram* build_program(const std::vector<std::string>& files,
	                                bool link_base =true);

	// This will reload and build and link all shaders and all programs
	void reload_all_shaders();

	// manually start and finish a compilation cycle
	// We will keep track of error statistics during a whole compilation cycle
	void start_compilation_cycle();
	void finish_compilation_cycle();
};
