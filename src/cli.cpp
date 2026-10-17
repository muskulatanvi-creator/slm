#include "cli.h"

#include <cctype>
#include <cmath>
#include <stdexcept>
#include <string>

namespace {

std::size_t parsePositiveInt(const std::string& s, const std::string& what) {
    if (s.empty() || s.find_first_not_of("0123456789") != std::string::npos)
        throw std::invalid_argument(what + " must be a positive integer, got '" + s + "'");
    std::size_t value = 0;
    try {
        value = std::stoull(s);
    } catch (const std::out_of_range&) {
        throw std::invalid_argument(what + " is too large: '" + s + "'");
    }
    if (value == 0)
        throw std::invalid_argument(what + " must be a positive integer, got '" + s + "'");
    return value;
}

double parseAlpha(const std::string& s) {
    double value = 0.0;
    std::size_t pos = 0;
    try {
        value = std::stod(s, &pos);
    } catch (const std::exception&) {
        throw std::invalid_argument("option -a needs a non-negative number, got '" + s + "'");
    }
    if (pos != s.size() || !std::isfinite(value) || value < 0)
        throw std::invalid_argument("option -a needs a non-negative number, got '" + s + "'");
    return value;
}

// An argument is an option if it starts with '-' followed by something that
// is not the start of a number (so "-5" is treated as a bad number, not an option).
bool looksLikeOption(const std::string& a) {
    return a.size() > 1 && a[0] == '-' &&
           !std::isdigit(static_cast<unsigned char>(a[1])) && a[1] != '.';
}

}  // namespace

Options parseArguments(int argc, char* argv[]) {
    const std::string usage = "usage: slm [-a pseudocount] n file length";
    Options opts;
    int i = 1;
    while (i < argc && looksLikeOption(argv[i])) {
        std::string opt = argv[i];
        if (opt == "-a") {
            if (i + 1 >= argc)
                throw std::invalid_argument("option -a needs a non-negative number");
            opts.alpha = parseAlpha(argv[i + 1]);
            i += 2;
        } else {
            throw std::invalid_argument("unknown option '" + opt + "'\n" + usage);
        }
    }
    if (argc - i != 3)
        throw std::invalid_argument("expected 3 arguments, got " + std::to_string(argc - i) +
                                    "\n" + usage);

    opts.n = parsePositiveInt(argv[i], "n-gram size");
    opts.file = argv[i + 1];
    opts.length = parsePositiveInt(argv[i + 2], "output length");
    if (opts.length < opts.n)
        throw std::invalid_argument("output length (" + std::to_string(opts.length) +
                                    ") must be at least n (" + std::to_string(opts.n) + ")");
    return opts;
}
