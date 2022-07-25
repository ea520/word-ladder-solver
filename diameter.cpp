#include <fstream>
#include "json.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "argparse.h"
#include <chrono>
#define FMT_HEADER_ONLY
#include "fmt/color.h"
#include "solver.h"
std::unordered_map<std::string, int> get_distances_bfs(const std::string &start, const nlohmann::json &graph);
std::vector<std::vector<std::string>> get_subgraphs(const std::vector<std::string> &words, const nlohmann::json &graph);

int main(int argc, char *argv[])
{
    using json = nlohmann::json;
    argparse::ArgumentParser program("bfs");
    program.add_argument("-n")
        .help("Word length")
        .scan<'i', int>()
        .default_value<int>(2);
    program.add_argument("--graph")
        .help("The path to the graph json file")
        .default_value<std::string>(std::string("graph.json"));

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

    const int n = program.get<int>("-n");
    const json data = json::parse(std::ifstream(program.get<std::string>("--graph")));

    const std::vector<std::string> names = [&data](size_t n)
    {
        std::vector<std::string> names;
        for (const auto &[key, value] : data.items())
        {
            if (key.size() == n)
                names.push_back(key);
        }
        return names;
    }(n);
    struct path_t
    {
        std::string left, right;
        int dist = 0;
        bool operator<(const path_t &right)
        {
            return dist < right.dist;
        }
    };

    auto subgraphs = get_subgraphs(names, data);
    using string_vec = const std::vector<std::string> &;

    std::sort(subgraphs.begin(), subgraphs.end(), [](string_vec left, string_vec right)
              { return left.size() > right.size(); });

    path_t max_dist;
    int i = 0;
    for (string_vec subgraph : subgraphs)
    {
        fmt::print("SUBGRAPH {}/{}\n", ++i, subgraphs.size());
        std::vector<path_t>
            dists((size_t)subgraph.size());
        int itters = 0;
#pragma omp parallel for
        for (size_t i = 0; i < subgraph.size(); i++)
        {
            auto distances = get_distances_bfs(subgraph[i], data);
            auto max_el = std::max_element(distances.begin(), distances.end());
            path_t p;
            p.left = subgraph[i];
            p.right = max_el->first;
            p.dist = max_el->second;
            dists[i] = p;
            int progress = 100 * itters++ / names.size();
            fmt::print("[{:#>{}}] {}%\r", "", progress, progress);
        }
        fmt::print("[{:#>{}}] {}%\n", "", 100, 100);

        auto max_el = std::max_element(dists.begin(), dists.end(), [](const path_t &left, path_t &right)
                                       { return left.dist < right.dist; });
        if (max_dist < *max_el)
            max_dist = *max_el;
    }
    std::cout << max_dist.left << " -> " << max_dist.right << " " << max_dist.dist << std::endl;
}

std::vector<std::vector<std::string>> get_subgraphs(const std::vector<std::string> &_words, const nlohmann::json &graph)
{
    std::vector<std::vector<std::string>> ret;
    std::unordered_set<std::string> words(_words.begin(), _words.end());
    size_t N = _words.size();
    fmt::print("FINDING SUBGRAPHS\n");
    while (words.size())
    {
        const std::string &start = *words.begin();
        auto distances = get_distances_bfs(start, graph);
        std::vector<std::string> subgraph = {start};
        std::unordered_set<std::string> new_words;
        for (const std::string &word : words)
        {
            if (distances[word] >= 0)
            {
                subgraph.push_back(std::move(word));
            }
            else
            {
                new_words.insert(std::move(word));
            }
        }
        words = new_words;
        ret.push_back(std::move(subgraph));
        int progress = 100 - words.size() * 100 / N;
        // print `progress` hashes in square brackets followed by the progress percentage
        // e.g.[###] 3%
        fmt::print("[{:#>{}}] {}%\r", "", progress, progress);
    }
    fmt::print("[{:#>{}}] {}%\n", "", 100, 100);
    return ret;
}