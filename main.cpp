#include "solver.h"
#include "argparse.h"
#include <chrono>
#include <cstdarg>
std::vector<std::string> words;
std::list<const std::string *> unvisited;
std::unordered_map<const std::string *, size_t> cache;
bool dijkstra = true;
size_t word_length;

#ifndef NDEBUG
template <typename... Args>
int logf(const char *f, Args... args)
{
    return printf(f, args...);
}
#else
template <typename... Args>
int logf(const char *f, Args... args)
{
    return 0;
}
#endif

int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("weaver");
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
    dijkstra = program.get<bool>("-d");

    // swap end and start as this implementation makes it easier to print from end to start
    const std::string _start = to_upper(program.get<std::string>("-e"));
    const std::string _end = to_upper(program.get<std::string>("-s"));

    if (_end.size() != _start.size())
    {
        std::cerr << "Start and end words must have the same length" << std::endl;
        std::exit(1);
    }

    word_length = _end.length();
    words = load_words(filename);

    auto start = std::find(words.begin(), words.end(), _start);
    auto end = std::find(words.begin(), words.end(), _end);
    if (end == words.end())
    {
        std::cerr << "The starting word (" << _end << ") is not in the word list\n";
        std::exit(1);
    }

    if (start == words.end())
    {
        std::cerr << "The ending word (" << _start << ") is not in the word list\n";
        std::exit(1);
    }

    logf("Searching for paths from %s to %s ...\n", end.base()->c_str(), start.base()->c_str());

    for (const std::string &word : words)
    {
        unvisited.push_back(&word);
    }

    get_distances(*start.base(), *end.base());
    node_t root = {{}, end.base()};
    make_tree(*start.base(), root);
    size_t visited_count = std::count_if(cache.begin(), cache.end(), [](std::pair<const std::string *, size_t> s)
                                         { return s.second != SIZE_MAX; });
    logf("There are %llu known reachable words\n", visited_count);

    print_tree(root);
    std::cout << std::endl;
}
