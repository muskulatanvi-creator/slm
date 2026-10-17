#include "TextGenerator.h"

#include <stdexcept>
#include <utility>

TextGenerator::TextGenerator(const NgramModel& model, std::mt19937 rng)
    : model_(model), rng_(std::move(rng)) {}

std::string TextGenerator::generate(std::size_t length) {
    if (length < model_.order())
        throw std::invalid_argument("output length must be at least " +
                                    std::to_string(model_.order()));

    std::string output = model_.sampleStart(rng_);
    std::string window = output;
    while (output.size() < length) {
        auto next = model_.sampleNext(window, rng_);
        if (!next) break;                       // stuck: give up
        output += *next;
        window.erase(0, 1);
        window += *next;
    }
    return output;
}
