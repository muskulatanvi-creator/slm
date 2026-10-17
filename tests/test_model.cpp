// Checks the model against the worked examples in the assignment.
#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>

#include "NgramModel.h"
#include "TextGenerator.h"

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { ++failures; \
    std::cerr << "FAIL line " << __LINE__ << ": " #cond "\n"; } } while (0)
static bool near(double a, double b) { return std::fabs(a - b) < 1e-9; }

int main() {
    {   // Example 1, alpha = 0
        NgramModel m(2, 0);
        m.train("ababac");
        CHECK(near(m.frequency("ab"), 0.4));
        CHECK(near(m.frequency("ba"), 0.4));
        CHECK(near(m.frequency("ac"), 0.2));
        CHECK(near(m.probability("ab", 'a'), 1.0));
        CHECK(near(m.probability("ba", 'b'), 0.5));
        CHECK(near(m.probability("ba", 'c'), 0.5));
        CHECK(near(m.probability("ac", 'a'), 0.0));
    }
    {   // Example 1, alpha = 1
        NgramModel m(2, 1);
        m.train("ababac");
        CHECK(near(m.probability("ab", 'a'), 0.6));
        CHECK(near(m.probability("ab", 'b'), 0.2));
        CHECK(near(m.probability("ba", 'b'), 0.4));
        CHECK(near(m.probability("ac", 'c'), 1.0 / 3));
        CHECK(near(m.probability("cc", 'a'), 1.0 / 3));   // unseen n-gram: uniform
    }
    {   // Example 2
        NgramModel m(3, 0);
        m.train("Hubba bubba");
        CHECK(near(m.frequency("ubb"), 2.0 / 9));
        CHECK(near(m.frequency("Hub"), 1.0 / 9));
        CHECK(near(m.probability("bba", ' '), 1.0));
    }
    {   // Generator gets stuck: "ababac" with n=2 can end at "ac"
        NgramModel m(2, 0);
        m.train("ababac");
        std::mt19937 rng(1);
        TextGenerator g(m, rng);
        for (int i = 0; i < 200; ++i) {
            std::string s = g.generate(50);
            CHECK(s.size() >= 2 && s.size() <= 50);
        }
    }
    {   // alpha > 0 always reaches the requested length
        NgramModel m(2, 0.01);
        m.train("ababac");
        TextGenerator g(m, std::mt19937(2));
        CHECK(g.generate(100).size() == 100);
    }
    {   // Errors
        bool thrown = false;
        try { NgramModel m(3, 0); m.train("abc"); } catch (const std::runtime_error&) { thrown = true; }
        CHECK(thrown);
        thrown = false;
        try { NgramModel m(0, 0); } catch (const std::invalid_argument&) { thrown = true; }
        CHECK(thrown);
        thrown = false;
        try { NgramModel m(2, -1); } catch (const std::invalid_argument&) { thrown = true; }
        CHECK(thrown);
    }
    if (failures == 0) std::cout << "All tests passed\n";
    return failures == 0 ? 0 : 1;
}
