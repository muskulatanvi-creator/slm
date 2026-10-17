#ifndef TEXTGENERATOR_H
#define TEXTGENERATOR_H

#include <cstddef>
#include <random>
#include <string>

#include "NgramModel.h"

// Generates text from a trained NgramModel.
class TextGenerator {
public:
    TextGenerator(const NgramModel& model, std::mt19937 rng);

    // Generates up to `length` characters, including the starting n-gram.
    // The result is shorter if the generator gets stuck (alpha == 0 and the
    // current n-gram has no transitions). Throws std::invalid_argument if
    // length is smaller than the model order.
    std::string generate(std::size_t length);

private:
    const NgramModel& model_;
    std::mt19937 rng_;
};

#endif
