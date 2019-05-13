#include "util.h"

#include <stdio.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <cstdlib>


/****************************************************************************
 * UTILITY FUNCTIONS: warning output, gl error checking                     *
 ****************************************************************************/

/* Print a info message to stdout, use printf syntax. */
extern void info (const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stdout,format, args);
	va_end(args);
	fputc('\n', stdout);
	fflush(stdout);
}

/* Print a warning message to stderr, use printf syntax. */
extern void warn (const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr,format, args);
	va_end(args);
	fputc('\n', stderr);
	fflush(stderr);
}


/* print a glm matrix */
extern void print_matrix(const glm::mat4& m, const char *prefix)
{
	if (!prefix) {
		prefix="";
	}

	for (int i=0; i<4; i++) {
		info("%s: %f %f %f %f",
		     prefix,
		     static_cast<double>(m[0][i]),
		     static_cast<double>(m[1][i]),
		     static_cast<double>(m[2][i]),
		     static_cast<double>(m[3][i]));

	}
}

/* load a file into a string*/
std::string load_file(const std::string &path)
{
	std::ifstream t(path);
	if(!t.is_open())
		warn("Could not open file '%s'!",path.c_str());
	return std::string((std::istreambuf_iterator<char>(t)),
	                   std::istreambuf_iterator<char>());
}
