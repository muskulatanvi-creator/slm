#ifndef UTIL_H
#define UTIL_H

#include <string>

// Reads a whole file into a string. Throws std::runtime_error if the file
// cannot be opened or read.
std::string readFile(const std::string& path);

#endif
