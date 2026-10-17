#include "util.h"

#include <fstream>
#include <iterator>
#include <stdexcept>

std::string readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in)
        throw std::runtime_error("cannot open file '" + path + "'");
    std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (in.bad())
        throw std::runtime_error("error while reading file '" + path + "'");
    return text;
}
