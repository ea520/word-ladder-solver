#include <fstream>
#include "json.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "argparse.h"
#include <unordered_set>
#include <queue>
#define FMT_HEADER_ONLY
#include "fmt/color.h"
#include "solver.h"

std::vector<std::vector<int32_t>> get_subgraphs(std::vector<int32_t> indexes, const std::vector<std::vector<int32_t>> &graph);
struct path_t
{
    std::string left, right;
    int dist = -1;
    bool operator<(const path_t &right)
    {
        return dist < right.dist;
    }
};
std::ostream &operator<<(std::ostream &stream, const path_t &path)
{
    stream << path.left << " -> " << path.right << "(path length:" << path.dist << ")";
    return stream;
}
int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("bfs");
    program.add_argument("-ns")
        .help("Word lengths")
        .scan<'i', int>()
        .default_value<std::vector<int>>({2, 3})
        .nargs(argparse::nargs_pattern::at_least_one);
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
    const std::vector<int> ns = program.get<std::vector<int>>("-ns");

    using json = nlohmann::json;
    const json data = json::parse(std::ifstream(program.get<std::string>("--graph")));
    const std::vector<std::string> names = data["names"];
    const std::vector<std::vector<int32_t>> graph = data["graph"];
    for (int n : ns)
    {
        std::vector<int32_t> indexes;
        for (int i = 0; i < names.size(); i++)
            if (names[i].size() == n || n == -1)
                indexes.push_back(i);

        auto subgraphs = get_subgraphs(indexes, graph);

        std::sort(subgraphs.begin(), subgraphs.end(), [](const auto &left, const auto &right)
                  { return left.size() > right.size(); });

        path_t max_dist;
        int subgraph_idx = 0;
        for (std::vector<int32_t> subgraph : subgraphs)
        {
            if (max_dist.dist > (int)subgraph.size())
                break;

            subgraph_idx++;
            fflush(nullptr);
            std::vector<path_t>
                dists((size_t)subgraph.size());
            int itters = 0;
#pragma omp parallel for
            for (size_t i = 0; i < subgraph.size(); i++)
            {
                auto distances = get_distances_bfs(subgraph[i], subgraph, graph);
                using pair_type = decltype(distances)::value_type;
                auto max_el = std::max_element(distances.begin(), distances.end());
                path_t p;
                p.left = names[subgraph[i]];
                p.right = names[max_el - distances.begin()];
                p.dist = *max_el;
                dists[i] = p;
                double progress = 100. * itters++ / subgraph.size();
                if (itters % 10 == 0)
                {
                    fmt::print("SUBGRAPH {}/{}: [{:#>{}}] {:.2f}%\r", subgraph_idx, subgraph.size(), "", (int)progress / 2, progress);
                    fflush(nullptr);
                }
            }
            fmt::print("{: >{}}\r", "", 90);

            auto max_el = std::max_element(dists.begin(), dists.end());
            if (max_dist < *max_el)
                max_dist = *max_el;
        }
        std::cout << max_dist << std::endl;
    }
}

std::unordered_set<int32_t> get_subgraphs(int32_t word, const std::vector<std::vector<int32_t>> &graph)
{
    std::unordered_set<int32_t> visited, _new({word});
    while (_new.size())
    {
        std::unordered_set<int32_t> temp;
        for (int32_t i : _new)
            if (visited.find(i) == visited.end())
                temp.insert(graph[i].begin(), graph[i].end());
        visited.insert(_new.begin(), _new.end());
        _new = std::move(temp);
    }
    return visited;
}
std::vector<std::vector<int32_t>> get_subgraphs(std::vector<int32_t> _words, const std::vector<std::vector<int32_t>> &graph)
{
    std::vector<std::vector<int32_t>> ret;
    std::unordered_set<int32_t> words(_words.begin(), _words.end());
    size_t N = words.size();

    while (words.size())
    {
        auto subgraph = get_subgraphs(*words.begin(), graph);
        for (int32_t word : subgraph)
        {
            words.erase(word);
        }
        ret.emplace_back(subgraph.begin(), subgraph.end());
        double progress = 100. - words.size() * 100. / N;
        // print `progress` hashes in square brackets followed by the progress percentage
        // e.g.[###] 3%
        fmt::print("FINDING SUBGRAPHS: [{:#>{}}{: >{}}] {:.2f}%\r", "", (int)progress / 2, "", 50 - (int)progress / 2, progress);
        fflush(nullptr);
    }
    fmt::print("{: >{}}\r", "", 90);
    // fflush(nullptr);
    return ret;
}