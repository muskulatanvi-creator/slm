#ifndef NGRAMMODEL_H
#define NGRAMMODEL_H

#include <cstddef>
#include <map>
#include <optional>
#include <random>
#include <set>
#include <string>
#include <vector>

// A character-level n-gram language model.
//
// Training collects (1) how often each n-gram occurs and (2) how often each
// character follows each n-gram. A pseudocount alpha >= 0 is added to every
// transition count, over the alphabet seen in the training text.
class NgramModel {
public:
    // Throws std::invalid_argument if n == 0 or alpha is negative/NaN.
    NgramModel(std::size_t n, double alpha);

    // Builds the model from text. Throws std::runtime_error if the text is too
    // short to contain any n-gram followed by a character.
    void train(const std::string& text);

    std::size_t order() const { return n_; }
    double alpha() const { return alpha_; }
    std::size_t alphabetSize() const { return alphabet_.size(); }

    // Relative frequency f(g) of an n-gram among all n-grams in the text.
    double frequency(const std::string& gram) const;

    // P(c | gram) including the pseudocount. Returns 0 when the distribution
    // is undefined (alpha == 0 and gram is never followed by anything).
    double probability(const std::string& gram, char c) const;

    // Draws a starting n-gram according to the n-gram frequencies.
    std::string sampleStart(std::mt19937& rng) const;

    // Draws the character following gram. Returns std::nullopt if the model
    // has no transitions from gram (only possible when alpha == 0).
    std::optional<char> sampleNext(const std::string& gram, std::mt19937& rng) const;

private:
    std::size_t n_;
    double alpha_;
    bool trained_ = false;

    std::vector<char> alphabet_;                          // sorted distinct chars
    std::map<std::string, std::size_t> gramCount_;        // occurrences of each n-gram
    std::size_t gramTotal_ = 0;                           // number of n-gram positions
    std::map<std::string, std::map<char, std::size_t>> transitions_;  // c(x | g)
    std::map<std::string, std::size_t> transitionTotal_;  // N(g)

    std::vector<std::string> startGrams_;
    mutable std::discrete_distribution<std::size_t> startDist_;
};

#endif
