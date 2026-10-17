#include "NgramModel.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

NgramModel::NgramModel(std::size_t n, double alpha) : n_(n), alpha_(alpha) {
    if (n == 0)
        throw std::invalid_argument("n-gram size must be a positive integer");
    if (std::isnan(alpha) || alpha < 0)
        throw std::invalid_argument("pseudocount must be a non-negative number");
}

void NgramModel::train(const std::string& text) {
    if (text.size() <= n_)
        throw std::runtime_error("training text is too short: it must contain at least one "
                                 + std::to_string(n_) + "-gram followed by a character");

    gramCount_.clear();
    transitions_.clear();
    transitionTotal_.clear();

    std::set<char> alphabet(text.begin(), text.end());
    alphabet_.assign(alphabet.begin(), alphabet.end());

    gramTotal_ = text.size() - n_ + 1;
    for (std::size_t i = 0; i + n_ <= text.size(); ++i) {
        std::string gram = text.substr(i, n_);
        ++gramCount_[gram];
        if (i + n_ < text.size()) {          // a character follows this occurrence
            ++transitions_[gram][text[i + n_]];
            ++transitionTotal_[gram];
        }
    }

    startGrams_.clear();
    std::vector<double> weights;
    for (const auto& [gram, count] : gramCount_) {
        startGrams_.push_back(gram);
        weights.push_back(static_cast<double>(count));
    }
    startDist_ = std::discrete_distribution<std::size_t>(weights.begin(), weights.end());
    trained_ = true;
}

double NgramModel::frequency(const std::string& gram) const {
    auto it = gramCount_.find(gram);
    if (it == gramCount_.end() || gramTotal_ == 0) return 0.0;
    return static_cast<double>(it->second) / static_cast<double>(gramTotal_);
}

double NgramModel::probability(const std::string& gram, char c) const {
    std::size_t total = 0, count = 0;
    if (auto it = transitionTotal_.find(gram); it != transitionTotal_.end()) {
        total = it->second;
        count = transitions_.at(gram).count(c) ? transitions_.at(gram).at(c) : 0;
    }
    bool inAlphabet = std::binary_search(alphabet_.begin(), alphabet_.end(), c);
    if (alpha_ == 0.0) {
        if (total == 0) return 0.0;
        return static_cast<double>(count) / static_cast<double>(total);
    }
    if (!inAlphabet) return 0.0;
    return (count + alpha_) / (total + alpha_ * alphabet_.size());
}

std::string NgramModel::sampleStart(std::mt19937& rng) const {
    if (!trained_) throw std::logic_error("model has not been trained");
    return startGrams_[startDist_(rng)];
}

std::optional<char> NgramModel::sampleNext(const std::string& gram, std::mt19937& rng) const {
    if (!trained_) throw std::logic_error("model has not been trained");

    auto it = transitions_.find(gram);   // end() for n-grams never followed by anything

    if (alpha_ == 0.0) {
        if (it == transitions_.end()) return std::nullopt;   // stuck
        std::vector<char> chars;
        std::vector<double> weights;
        for (const auto& [c, count] : it->second) {
            chars.push_back(c);
            weights.push_back(static_cast<double>(count));
        }
        std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());
        return chars[dist(rng)];
    }

    // alpha > 0: every alphabet character gets weight count + alpha. For an
    // unseen n-gram all counts are 0, giving a uniform distribution.
    std::vector<double> weights;
    weights.reserve(alphabet_.size());
    for (char c : alphabet_) {
        double count = 0.0;
        if (it != transitions_.end()) {
            auto cit = it->second.find(c);
            if (cit != it->second.end()) count = static_cast<double>(cit->second);
        }
        weights.push_back(count + alpha_);
    }
    std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());
    return alphabet_[dist(rng)];
}
