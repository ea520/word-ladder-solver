#include "solver.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <fstream>
#include "argparse.h"
#include <chrono>
#include <cstdarg>
#include "json.hpp"
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
    program.add_argument("-g", "--graph").help("The graph for the words").default_value(std::string("graph.json"));

    program.add_argument("-s", "--start").help("The starting word in the game").required().default_value(std::string("SLOW"));

    program.add_argument("-e", "--end").help("The ending word in the game").required().default_value(std::string("FAST"));

    program.add_argument("-d", "--dijkstra").help("Whether to use Dijkstra or breadth first search").default_value(false).implicit_value(true);

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
    using json = nlohmann::json;
    const json data = json::parse(std::ifstream(program.get<std::string>("--graph")));
    const std::vector<std::string> words = data["names"];
    const std::vector<std::vector<int32_t>> graph = data["graph"];
    std::vector<int32_t> indexes;
    for (int i = 0; i < words.size(); i++)
        if (words[i].size() == _start.size())
            indexes.push_back(i);
    auto start_itter = std::lower_bound(words.begin(), words.end(), _start);
    auto end_itter = std::lower_bound(words.begin(), words.end(), _end);

    if (start_itter == words.end())
    {
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "The ending word ({}) is not in the word list\n", _end);
        std::exit(1);
    }

    if (end_itter == words.end())
    {
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "The starting word ({}) is not in the word list\n", _start);
        std::exit(1);
    }
    LOG(fg(fmt::color::yellow) | fmt::emphasis::bold, "Searching for paths from {} to {} ...\n", _start, _end);
    auto get_distances = [dijkstra](int32_t start, int32_t end, const std::vector<int32_t> &indexes, const std::vector<std::vector<int32_t>> &graph)
    {
        if (dijkstra)
            return get_distances_dijkstra(start, end, indexes, graph);
        else
            return get_distances_bfs(start, end, indexes, graph);
    };

    const auto distances = get_distances(start_itter - words.begin(), end_itter - words.begin(), indexes, graph);
    const auto path = get_path(end_itter - words.begin(), distances, indexes, graph);
    auto print_similarities = [](const std::string &to_print, const std::string &to_compare)
    {
        for (int i = 0; i < to_print.size(); i++)
            if (to_print[i] == to_compare[i])
                fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "{}", to_print[i]);
            else
                fmt::print(fg(fmt::color::yellow) | fmt::emphasis::bold, "{}", to_print[i]);
    };
    for (const int32_t node : path)
    {
        print_similarities(words[node], _end);
        if (node != end_itter - words.begin())
            fmt::print(" -> ");
    }
    std::cout << std::endl;
}