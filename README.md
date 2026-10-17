# slm – Small Language Model ("Artificial Idiot")

`slm` reads a text file, learns a character-level *n*-gram language model from it,
and uses the model to generate new, not necessarily correct, text.

## How it works

For a word size *n*, the model records from the training text

1. the relative frequency *f(g)* of every *n*-gram *g* (occurrences of *g* divided by the number of *n*-gram positions in the text), and
2. for every *n*-gram *g*, how many times *c(x | g)* each character *x* follows it.

The transition probability, with pseudocount *a* and alphabet *A* (distinct characters in the training text), is

    P(x | g) = (c(x | g) + a) / (N(g) + a·|A|)

where *N(g)* is the number of occurrences of *g* that are followed by a character
(an occurrence at the very end of the text is not counted). With `a = 0` this is
the plain maximum-likelihood estimate; with `a > 0` unseen *n*-grams get a uniform distribution.

Generation:

1. Pick a starting *n*-gram according to *f* and output it.
2. Sample the next character from `P(· | window)` using `std::discrete_distribution`.
   If `a = 0` and the window has no transitions (it only occurs at the end of the training text), stop early.
3. Output the character, slide the window one step, and repeat until the requested length
   (which includes the starting *n*-gram) is reached.

## Build

Requires a C++17 compiler (g++ or clang++) and `make`.

    make          # builds ./slm  (only recompiles what changed)
    make test     # builds and runs the unit tests (examples from the assignment)
    make clean    # removes build/, slm and the test binary

## Usage

    slm [-a pseudocount] n file length

* `n` – word size (positive integer)
* `file` – plain ASCII training text
* `length` – number of characters to output, including the starting *n*-gram (at least `n`)
* `-a` – pseudocount, a non-negative number (default 0); must come before the three arguments

Examples:

    $ slm 2 moby_dick.txt 5
    $ slm -a 0.01 3 moby_dick.txt 40

Note: with `-a 0` the output can be shorter than requested if the generator gets stuck.
Larger `n` gives more real-looking words (and, for small texts, more verbatim copying);
larger `a` gives more random output.

## Error handling

User errors are reported with standard exceptions (`std::invalid_argument`,
`std::runtime_error`), caught in `main`, printed to `std::cerr` as `slm: <message>`,
and the program exits with status 1. Handled cases: wrong number of arguments,
`n` or length not a positive integer, length smaller than `n`, `-a` without a
non-negative number, unknown option, unreadable training file, and a training text
too short to contain an *n*-gram followed by a character.

    $ slm 2 no_such_file.txt 100
    slm: cannot open file 'no_such_file.txt'

## Code layout

| File | Purpose |
|------|---------|
| `src/NgramModel.{h,cpp}` | Class `NgramModel`: training, frequencies, probabilities, sampling |
| `src/TextGenerator.{h,cpp}` | Class `TextGenerator`: the generation loop |
| `src/cli.{h,cpp}` | Command-line parsing and validation |
| `src/util.{h,cpp}` | `readFile` (throws if the file cannot be opened) |
| `src/main.cpp` | Entry point; catches exceptions |
| `tests/test_model.cpp` | Unit tests checking the worked examples |
| `Makefile` | Build rules, with automatic header dependency tracking |

## Design notes

* `std::map<std::string, ...>` stores the counts; the model is built in a single pass over the text.
* `NgramModel` knows nothing about I/O or randomness sources; the RNG is passed in, which makes it easy to reuse and test.
* For `a > 0` the weights over the whole alphabet are built on the fly, so memory use stays proportional to the number of distinct *n*-grams seen.
