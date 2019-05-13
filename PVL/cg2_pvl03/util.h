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

float random_float(const float min, const float max);
glm::vec3 random_vec3(const glm::vec3& min = glm::vec3(0.0f), const glm::vec3 max = glm::vec3(1.0f));


/*
 * H(Hue): 0 - 1.0 tau
 * S(Saturation): 0 - 1.00
 * V(Value): 0 - 1.00s
 */
glm::vec3 HSVtoRGB(glm::vec3 hsv);

/* returns a decent looking (not just greenish) color.*/
glm::vec3 random_clr(float sat_min =1, float sat_max = 1);


#endif
