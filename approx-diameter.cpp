#include <fstream>
#include "json.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <queue>
#include "argparse.h"
#include <chrono>
#define FMT_HEADER_ONLY
#include "fmt/color.h"
std::unordered_map<std::string, int> get_distances_bfs(const std::string &start, const nlohmann::json &graph);

int main(int argc, char *argv[])
{
    using json = nlohmann::json;
    argparse::ArgumentParser program("bfs");
    program.add_argument("-n")
        .help("Word length")
        .scan<'i', int>();
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
    auto to_upper = [](std::string input)
    {
        transform(input.begin(), input.end(), input.begin(), ::toupper);
        return input;
    };

    const int n = program.get<int>("-n");
    const json data = json::parse(std::ifstream(program.get<std::string>("--graph")));

    const std::vector<std::string> names = [&data](int n)
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
        int dist;
        bool operator<(const path_t &right)
        {
            return dist < right.dist;
        }
    };

    std::vector<path_t> dists((size_t)names.size());
    int itters = 0;
#pragma omp parallel for
    for (int i = 0; i < names.size(); i++)
    {
        auto distances = get_distances_bfs(names[i], data);
        auto max_el = std::max_element(distances.begin(), distances.end());
        path_t p;
        p.left = names[i];
        p.right = max_el->first;
        p.dist = max_el->second;
        dists[i] = p;
        printf("\r%.2lf%%   ", (double)itters++ / names.size() * 100.);
    }
    printf("\r%.2lf%%   \n", 100.);
    auto max_el = std::max_element(dists.begin(), dists.end(), [](const path_t &left, path_t &right)
                                   { return left.dist < right.dist; });
    std::cout << max_el->left << " -> " << max_el->right << " " << max_el->dist << std::endl;
}

std::unordered_map<std::string, int> get_distances_bfs(const std::string &start, const nlohmann::json &graph)
{
    std::unordered_map<std::string, int> distances;
    const int INF = 10000, length = start.size();
    for (const auto &[key, value] : graph.items())
    {
        if (key.size() == length)
            distances[key] = INF;
    }
    std::unordered_set<std::string> visited;
    std::queue<std::string> pending;
    distances[start] = 0;
    pending.push(start);
    std::string node;
    while (!pending.empty())
    {
        node = pending.front();
        visited.insert(node);
        pending.pop();
        std::vector<std::string>
            unvisited_neighbours(graph[node].begin(), graph[node].end());
        for (const std::string &un : unvisited_neighbours)
        {
            if (visited.find(un) == visited.end())
            {
                distances[un] = std::min(distances[un], distances[node] + 1);
                pending.push(un);
            }
        }
    }
    for (auto &[node, dist] : distances)
    {
        if (dist == INF)
            dist = -1;
    }
    return distances;
}