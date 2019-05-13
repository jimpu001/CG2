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

float random_float(const float min, const float max)
{
	return min + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(max-min)));
}

glm::vec3 random_vec3(const glm::vec3& min, const glm::vec3 max)
{
	glm::vec3 result;
	for(int i = 0 ; i< 3;i++){
		result[i] = random_float(min[i],max[i]);
	}
	return  result;
}

glm::vec3 HSVtoRGB(glm::vec3 hsv) {
	float C = hsv.y * hsv.z;
	float X = C * (1 - abs(fmodf(hsv.x / (1.0f/6.0f), 2) - 1));
	float m = hsv.z - C;
	float Rs, Gs, Bs;


	if(hsv.x >= 0 && hsv.x < 1.0f/6.0f) {
		Rs = C;
		Gs = X;
		Bs = 0;
	}
	else if(hsv.x >= 1.0f/6.0f && hsv.x < 1.0f/3.0f) {
		Rs = X;
		Gs = C;
		Bs = 0;
	}
	else if(hsv.x >= 1.0f/3.0f && hsv.x < 1.0f/2.0f) {
		Rs = 0;
		Gs = C;
		Bs = X;
	}
	else if(hsv.x >= 1.0f/2.0f && hsv.x < 2.0f/3.0f) {
		Rs = 0;
		Gs = X;
		Bs = C;
	}
	else if(hsv.x >= 2.0f/3.0f && hsv.x < 5.0f/6.0f) {
		Rs = X;
		Gs = 0;
		Bs = C;
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;
	}

	return glm::vec3((Rs + m), (Gs + m), (Bs + m));
}

glm::vec3 random_clr(float sat_min, float sat_max)
{
	return HSVtoRGB(glm::vec3(random_float(0,1),random_float(sat_min,sat_max),1.0f));
}
