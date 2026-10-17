#ifndef CLI_H
#define CLI_H

#include <cstddef>
#include <string>

struct Options {
    std::size_t n = 0;       // n-gram size
    std::string file;        // training file
    std::size_t length = 0;  // number of characters to output
    double alpha = 0.0;      // pseudocount
};

// Parses the command line: slm [-a pseudocount] n file length
// Throws std::invalid_argument on any user error.
Options parseArguments(int argc, char* argv[]);

#endif
