#include "shader_compositor.h"



GLuint CG2ShaderCompositor::load_and_compile(const std::string &path, GLuint shader)
{

	std::string ending = path.substr(path.find_last_of('.')+1);
	std::transform(ending.begin(), ending.end(), ending.begin(), ::tolower);
	std::ifstream f(path);
	if(!f.is_open())
	{
		warn("Could not open file:'%s'",path.c_str());
	}
	std::string code((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

	GLenum shader_type = 0;
	if (ending == "fs" || ending == "frag")
	{
		shader_type = GL_FRAGMENT_SHADER;
	}
	else if (ending == "vs" || ending == "vert")
	{
		shader_type = GL_VERTEX_SHADER;
	}
	else if (ending == "cs" || ending == "comp")
	{
		shader_type = GL_COMPUTE_SHADER;
	}
	else if (ending == "geom")
	{
		shader_type = GL_GEOMETRY_SHADER;
	}

	if(!shader)
		shader = glCreateShader(shader_type);

	m_shaders[path] = shader;
	m_shader_paths[shader] = path;

	const GLchar* code_ptr = (code.c_str());
	glShaderSource(shader, 1, &code_ptr, nullptr);

	info("Compiling shader '%s' ... ", path.c_str());
	glCompileShader(shader);

	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

	GLint maxLength = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	// The maxLength includes the NULL character
	std::string errorLog;
	errorLog.resize(static_cast<size_t>(maxLength));
	glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);


	if (isCompiled == GL_FALSE)
	{
		if (!first_failed_shader) {
			first_failed_shader.identifier=std::to_string(shader)+": '" + path + "'";
			first_failed_shader.message=errorLog;
		}
		if (maxLength)
			warn("\n%s\n", errorLog.c_str());
		warn(" ...  compiling Failed!\n");
	}
	else
	{
		if (maxLength)
			info("\n%s\n", errorLog.c_str());
		info("... Done!\n");
	}

	return shader;
}

bool CG2ShaderCompositor::link_program(GLuint prog)
{
	glLinkProgram(prog);

	GLint is_linked = 0;
	glGetProgramiv(prog, GL_LINK_STATUS, &is_linked);

	GLint maxLength = 0;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &maxLength);

	// The maxLength includes the NULL character
	std::string errorLog;
	errorLog.resize(static_cast<size_t>(maxLength));
	glGetProgramInfoLog(prog, maxLength, &maxLength, &errorLog[0]);

	std::string name=get_program_name(prog);
	info("Linking Program ...\n%s",name.c_str());

	if (is_linked == GL_FALSE)
	{
		if (!first_failed_program) {
			first_failed_program.identifier=get_program_name(prog);
			first_failed_program.message=errorLog;
		}
		if (maxLength)
			warn("\n%s\n", errorLog.c_str());
		warn( "... Failed!\n");
	}
	else
	{
		if (maxLength)
			info("\n%s\n", errorLog.c_str());
		info("... Done!\n");

	}
	return (is_linked == GL_TRUE);
}

std::string CG2ShaderCompositor::get_shader_name(GLuint shader) const
{
	auto it = m_shader_paths.find(shader);
	if(it != m_shader_paths.end())
	{
		const auto& path = (*it).second;
		return  std::to_string(shader)+": "+path.substr(path.find_last_of('/')+1);
	}
	else
	{
		return std::to_string(shader)+": Unknown Shader";
	}
}

CG2ShaderProgram *CG2ShaderCompositor::link_program(const ProgramIdentifier &pi, bool link_bases)
{
	CG2ShaderProgram* res = new CG2ShaderProgram();

	res->link_bases = link_bases;
	if (link_bases)
	{
		for (int i = 0; i < BVTX_COUNT; i++)
			for (int j = 0; j < BFRG_COUNT; j++) {
				GLuint prog = glCreateProgram();
				for (const auto s : pi) {
					glAttachShader(prog, s);
				}

				glAttachShader(prog, m_base_vtx_shader[i]);
				glAttachShader(prog, m_base_frg_shader[j]);

				m_raw_programs.insert(prog);
				m_raw_to_program[prog] = res;
				m_raw_to_program_id[prog] = pi;
				res->m_programs[i + BVTX_COUNT * j] = prog;
				link_program(prog);
			}
	}else{
		GLuint prog = glCreateProgram();
		for (const auto s : pi)
		{
			glAttachShader(prog, s);
		}
		m_raw_programs.insert(prog);
		m_raw_to_program[prog]=res;
		m_raw_to_program_id[prog]=pi;
		link_program(prog);
		for(int i = 0; i< BVTX_COUNT;i++)
			for(int j = 0; j< BFRG_COUNT;j++){
				res->m_programs[i+BVTX_COUNT*j] = prog;
			}
	}
	return res;
}

GLuint CG2ShaderCompositor::get_shader(const std::string &path)
{
	if (m_shaders.find(path) != m_shaders.end())
		return m_shaders[path];
	return load_and_compile(path);
}

CG2ShaderProgram *CG2ShaderCompositor::get_program(const ProgramIdentifier &pi, bool link_base)
{
	if (m_programs.find(pi) == m_programs.end())
	{
		m_programs[pi] = link_program(pi,link_base);
	}
	return m_programs[pi];
}

std::string CG2ShaderCompositor::get_program_name(const GLuint prog)
{
	const auto pid = m_raw_to_program_id[prog];
	std::string res = "{";
	res.reserve(128);
	for(const auto& s : pid){
		res+=get_shader_name(s)+";";
	}
	res+="}";
	return res;
}

CG2ShaderCompositor::CG2ShaderCompositor()
{
	start_compilation_cycle();
}

CG2ShaderCompositor::~CG2ShaderCompositor()
{
	clear();
}

void CG2ShaderCompositor::init()
{
	// load the base shader
	m_base_vtx_shader[BVTX_FULL] =  load_and_compile( "data/shaders/base_vtx_full.vert");
	m_base_vtx_shader[BVTX_POSITION_ONLY] =  load_and_compile( "data/shaders/base_vtx_position_only.vert");

	m_base_frg_shader[BFRG_NULL] =  load_and_compile( "data/shaders/base_frg_null.frag");
	m_base_frg_shader[BFRG_WRITE_COLORBUFF] =  load_and_compile( "data/shaders/base_frg_write_color_buffer.frag");
	m_base_frg_shader[BFRG_GHOST] =  load_and_compile( "data/shaders/base_frg_null.frag");
	m_base_frg_shader[BFRG_SSPREPASS] =  load_and_compile( "data/shaders/base_frg_null.frag");
}

void CG2ShaderCompositor::clear()
{
	for (auto& p : m_raw_programs)
	{
		glDeleteProgram(p);
	}
	m_raw_programs.clear();
	for (const auto& s : m_shaders)
	{
		glDeleteShader(s.second);
	}
	m_shaders.clear();
	m_shader_paths.clear();

	for(auto & p : m_programs)
	{
		delete  p.second;
	}
	m_programs.clear();
	m_raw_to_program.clear();
	m_raw_to_program_id.clear();
}

CG2ShaderProgram *CG2ShaderCompositor::build_program(const std::vector<std::string> &files, bool link_base)
{
	ProgramIdentifier pi;
	for(const auto& p : files)
	{
		pi.insert(get_shader(p));
	}
	return get_program(pi,link_base);
}

void CG2ShaderCompositor::reload_all_shaders()
{
	start_compilation_cycle();
	for(auto& s: m_shaders)
	{
		load_and_compile(s.first,s.second); // rebuild all shaders
	}

	for(auto& prog : m_raw_programs)
	{
		link_program(prog);
	}
	finish_compilation_cycle();
}

void CG2ShaderCompositor::start_compilation_cycle()
{
	first_failed_shader.reset();
	first_failed_program.reset();
}

void CG2ShaderCompositor::finish_compilation_cycle()
{
	info("shader compositor: combined %u shaders into %u programs", m_shaders.size(), m_raw_programs.size());
	if (first_failed_shader) {
		warn("shader compositor: at least one shaders FAILED TO COMPILE:");
		warn("%s\n%s",first_failed_shader.identifier.c_str(),first_failed_shader.message.c_str());
	}
	if (first_failed_program) {
		warn("shader compositor: at least one program FAILED TO LINK:");
		warn("%s\n%s",first_failed_program.identifier.c_str(),first_failed_program.message.c_str());
	}
}
