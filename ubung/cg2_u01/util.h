#pragma once
#ifndef CG2_UTIL_H
#define CG2_UTIL_H
#include <stdarg.h>
#include <string>
#include "glad/glad.h"
#include "glm/mat4x4.hpp"

/* tau = 2 * pi */
#define CG2_TAUf 6.283185307179586476925286766559f
#define CG2_TAUd 6.283185307179586476925286766559005768394338798750211641949

/* Print a info message to stdout, use printf syntax. */
extern void info (const char *format, ...);

/* Print a warning message to stderr, use printf syntax. */
extern void warn (const char *format, ...);

/* print a glm matrix */
extern void print_matrix(const glm::mat4& m, const char *prefix=nullptr);

/* loads the file path from disk to memory */
extern std::string load_file(const std::string& path);



#endif
