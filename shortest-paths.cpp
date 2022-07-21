#include "solver.h"
#include "argparse.h"
#include <chrono>
#include <cstdarg>
#define FMT_HEADER_ONLY
#include "fmt/color.h"

#ifndef NDEBUG
template <typename... Args>
void LOG(Args... args)
{
    fmt::print(args...);
}
#else
template <typename... Args>
void LOG(Args... args)
{
}
#endif

int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("shortest-path");
    program.add_argument("-l", "--word-list").help("Path to the (new-line-separated) list of keywords").default_value(std::string("word-list.txt"));

    program.add_argument("-s", "--start").help("The starting word in the game").required().default_value(std::string("SLOW"));

    program.add_argument("-e", "--end").help("The ending word in the game").required().default_value(std::string("FAST"));

    program.add_argument("-d", "--dijkstra").help("Whether to use Dijkstra (slower but gives all solutions) or A*").default_value(false).implicit_value(true);

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    std::string filename = program.get<std::string>("-l");
    bool dijkstra = program.get<bool>("-d");

    // swap end and start as this implementation makes it easier to print from end to start
    const std::string _start = to_upper(program.get<std::string>("-s"));
    const std::string _end = to_upper(program.get<std::string>("-e"));

    if (_end.size() != _start.size())
    {
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "Start and end words must have the same length\n");
        std::exit(1);
    }

    size_t word_length = _end.length();
    auto words = load_words(filename, word_length);
    auto predicate = [](const std::string *val, const std::string &value)
    {
        return *val == value;
    };
    auto start = std::find_if(words.begin(), words.end(), [_start, predicate](const std::string *val)
                              { return predicate(val, _start); });
    auto end = std::find_if(words.begin(), words.end(), [_end, predicate](const std::string *val)
                            { return predicate(val, _end); });
    if (end == words.end())
    {
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "The ending word ({}) is not in the word list\n", _end);
        std::exit(1);
    }

    if (start == words.end())
    {
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "The starting word ({}) is not in the word list\n", _start);
        std::exit(1);
    }
    LOG(fg(fmt::color::yellow) | fmt::emphasis::bold, "Searching for paths from {} to {} ...\n", **start, **end);

    auto cache = get_distances(*start, *end, words, dijkstra);
    auto comp = [](std::pair<const std::string *, size_t> left, std::pair<const std::string *, size_t> right)
    {
        size_t left_comp = left.second == INT_MAX ? 0 : left.second;
        size_t right_comp = right.second == INT_MAX ? 0 : right.second;
        return left_comp < right_comp;
    };
    if (cache[*end] == INT_MAX)
    {
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "No path exists between {} and {}\n", **start, **end);
    }
    size_t visited_count = std::count_if(cache.begin(), cache.end(), [](std::pair<const std::string *, size_t> s)
                                         { return s.second != INT_MAX; });
    LOG(fg(fmt::color::yellow) | fmt::emphasis::bold, "{} words have been visited ({:.2f}%)\n", visited_count, (double)visited_count / (double)cache.size() * 100.);

    auto path = get_path(**end, cache, words);
    fmt::print(fg(fmt::color::yellow) | fmt::emphasis::bold, "Path length: {}\n", path.size() - 1);
    for (const std::string *node : path)
    {
        size_t i = 0;
        for (char c : *node)
        {
            if ((**end)[i] == c)
                fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "{}", c);
            else
                fmt::print("{}", c);
            i++;
        }
        std::cout << (node != *end ? " -> " : "\n");
    }
    std::cout << std::endl;
}