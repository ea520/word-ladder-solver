#include "solver.h"
#include "argparse.h"
#include <chrono>
#include <cstdarg>
#define FMT_HEADER_ONLY
#include "fmt/color.h"
#include <unordered_set>

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
struct path_t
{
    const std::string *start, *end;
    size_t dist;
};
path_t longest_dist(const std::string *start, const std::vector<const std::string *> &words)
{
    const std::string *end = start;
    size_t last_dist = 0;
    std::unordered_map<const std::string *, size_t>::iterator furthest;
    while (true)
    {
        auto cache = get_distances(*start, *end, words, true);
        auto comp = [](std::pair<const std::string *, size_t> left, std::pair<const std::string *, size_t> right)
        {
            size_t left_comp = left.second == SIZE_MAX ? 0 : left.second;
            size_t right_comp = right.second == SIZE_MAX ? 0 : right.second;
            return left_comp < right_comp;
        };
        furthest = std::max_element(cache.begin(), cache.end(), comp);

        if (furthest->second == SIZE_MAX || furthest->second == 0)
            break;

        if (last_dist == furthest->second)
            break;
        last_dist = furthest->second;
        end = start;
        std::vector<const std::string *> words;
        for (auto [word, len] : cache)
            words.push_back(word);
        start = *std::find(words.begin(), words.end(), furthest->first);
    }
    return {start, end, furthest->second == SIZE_MAX ? 0 : furthest->second};
}

std::unordered_set<const std::string *> get_graph(const std::string *word, std::vector<const std::string *> words)
{
    std::unordered_set<const std::string *> visited_set = {word};
    std::unordered_set<const std::string *> new_words = {word};
    while (new_words.size() > 0)
    {
        std::unordered_set<const std::string *> temp;
        for (const std::string *s : new_words)
        {
            auto neighbours = get_neighbours(*s, words);
            for (const std::string *neighbour : neighbours)
                if (visited_set.find(neighbour) == visited_set.end())
                    temp.insert(neighbour);
        }
        new_words = temp;
        visited_set.insert(new_words.begin(), new_words.end());
    }
    return visited_set;
}
std::vector<std::vector<const std::string *>> get_unconnected_graphs(std::vector<const std::string *> words)
{
    std::vector<std::vector<const std::string *>> ret;
    LOG(fg(fmt::color::yellow) | fmt::emphasis::bold, "Finding the unconnected graphs...\n");
    fflush(stdout);
    size_t N = words.size();
    while (words.size() > 0)
    {
        LOG(fg(fmt::color::yellow) | fmt::emphasis::bold, "{:.2f}%\t\t\r", 100. * (1. - double(words.size()) / N));
        fflush(stdout);
        auto visited_set = get_graph(words[0], words);
        std::vector<const std::string *> graph(visited_set.begin(), visited_set.end());

        ret.push_back(graph);
        auto cond = [visited_set](const std::string *word) -> bool
        {
            bool b = visited_set.find(word) != visited_set.end();
            return b;
        };
        words.erase(std::remove_if(words.begin(), words.end(), cond), words.end());
    }
    LOG(fg(fmt::color::yellow) | fmt::emphasis::bold, "{:.2f}%\t\t\r\n", 100.);

    return ret;
}

int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("weaver");
    program.add_argument("-l", "--word-list").help("Path to the (new-line-separated) list of keywords").default_value(std::string("word-list.txt"));

    program.add_argument("-n", "--count").help("the number of letters").required().default_value(4).scan<'i', int>();

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

    size_t word_length = program.get<int>("-n");
    auto words = load_words(filename, word_length);
    if (words.size() == 0)
    {
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "No words of length {} were found in the dictionary", word_length);
        std::exit(1);
    }

    const std::string *max_start, *max_end;
    std::vector<std::vector<const std::string *>> unconnected_graphs = get_unconnected_graphs(words);
    std::sort(unconnected_graphs.begin(), unconnected_graphs.end(), [](const std::vector<const std::string *> &v1, const std::vector<const std::string *> &v2)
              { return v1.size() > v2.size(); });
    fmt::print(fg(fmt::color::yellow) | fmt::emphasis::bold, "Finding longest optimal path...\n", word_length);
    size_t words_seen = 0, max_dist = 0;

    for (size_t i = 0; i < unconnected_graphs.size(); i++)
    {
        const auto &_words = unconnected_graphs[i];
        if (_words.size() < max_dist)
            break;
        words_seen += _words.size();
        LOG(fg(fmt::color::yellow) | fmt::emphasis::bold, "{:.2f}%\t\t\r", 100. * double(words_seen) / double(words.size()));
        fflush(stdout);
        auto [_start, _end, dist] = longest_dist(_words[0], _words);
        if (dist > max_dist)
        {
            max_dist = dist;
            max_start = _start;
            max_end = _end;
        }
    }
    LOG(fg(fmt::color::yellow) | fmt::emphasis::bold, "{:.2f}%\t\t\t\n", 100.);
    auto graph = get_graph(max_start, words);
    words.clear();
    words.insert(words.begin(), graph.begin(), graph.end());

    auto distances = get_distances(*max_start, *max_end, words, false);
    auto paths = get_paths(*max_end, distances, words);
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "PATH LENGTH: {}\n", paths[0].size());
    auto &path = paths[0];
    for (const std::string *node : path)
    {
        size_t i = 0;
        for (char c : *node)
        {
            if ((*max_end)[i] == c)
                fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "{}", c);
            else
                fmt::print("{}", c);
            i++;
        }
        std::cout << (node != max_end ? " -> " : "\n\n");
    }
}