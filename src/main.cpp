// slm - Small Language Model: an "Artificial Idiot".
//
// Usage: slm [-a pseudocount] n file length

#include <exception>
#include <iostream>
#include <random>

#include "NgramModel.h"
#include "TextGenerator.h"
#include "cli.h"
#include "util.h"

int main(int argc, char* argv[]) {
    try {
        Options opts = parseArguments(argc, argv);
        std::string text = readFile(opts.file);

        NgramModel model(opts.n, opts.alpha);
        model.train(text);

        std::random_device rd;
        TextGenerator generator(model, std::mt19937(rd()));
        std::cout << generator.generate(opts.length) << '\n';
    } catch (const std::exception& e) {
        std::cerr << "slm: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
