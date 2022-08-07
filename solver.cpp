#include "solver.h"
#include <unordered_set>
#include <queue>
#include <iostream>
std::string to_upper(std::string input)
{
    transform(input.begin(), input.end(), input.begin(), ::toupper);
    return input;
}

std::vector<int32_t> get_distances_bfs(const int32_t start, const int32_t end, const std::vector<int32_t> &indexes, const std::vector<std::vector<int32_t>> &graph)
{
    std::vector<int32_t> distances(graph.size(), graph.size());
    const int INF = graph.size();
    // for (int32_t index : indexes)
    //     distances[index] = INF;
    std::unordered_set<int32_t> visited;
    std::queue<int32_t> pending;
    distances[start] = 0;
    pending.push(start);
    int node;
    while (!pending.empty())
    {
        node = pending.front();
        if (node == end)
            break;
        visited.insert(node);
        pending.pop();
        for (const int un : graph[node])
        {
            if (visited.find(un) == visited.end() && distances[un] == INF)
            {
                distances[un] = distances[node] + 1;
                pending.push(un);
            }
        }
    }
    for (auto &dist : distances)
    {
        if (dist == INF)
            dist = -1;
    }
    return distances;
}

std::vector<int32_t> get_distances_dijkstra(const int32_t start, const int32_t end, const std::vector<int32_t> &indexes, const std::vector<std::vector<int32_t>> &graph)
{
    std::vector<int32_t> distances(graph.size(), graph.size());
    const int INF = graph.size();
    for (int32_t index : indexes)
        distances[index] = INF;
    std::unordered_set<int32_t> unvisited(indexes.begin(), indexes.end());
    distances[start] = 0;
    while (unvisited.size())
    {
        auto pred = [&](const int32_t left, const int32_t right)
        { return distances.at(left) < distances.at(right); };
        auto min = std::min_element(unvisited.begin(), unvisited.end(), pred);
        if (min != unvisited.end() && (distances.at(*min) == INF && *min != end))
        {
            break;
        }
        for (const int32_t neighbour : graph[*min])
        {
            distances[neighbour] = std::min(distances[neighbour], distances[*min] + 1);
        }
        unvisited.erase(*min);
    }
    for (auto &dist : distances)
    {
        if (dist == INF)
            dist = -1;
    }
    return distances;
}

std::vector<int32_t> get_distances_dijkstra(const int32_t start, const std::vector<int32_t> &indexes, const std::vector<std::vector<int32_t>> &graph)
{
    return get_distances_dijkstra(start, -1, indexes, graph);
}

std::vector<int32_t> get_distances_bfs(const int32_t start, const std::vector<int32_t> &indexes, const std::vector<std::vector<int32_t>> &graph)
{
    return get_distances_bfs(start, -1, indexes, graph);
}

std::vector<int32_t> get_path(const int node, const std::vector<int32_t> &distances, const std::vector<int32_t> &indexes, const std::vector<std::vector<int32_t>> &graph)
{
    size_t my_dist = distances[node];

    if (my_dist == indexes.size())
    {
        puts("An error has occurred");
        std::exit(1);
    }
    else if (my_dist == 0)
    {
        return {node};
    }
    auto neighbours = graph[node];
    int next = -1;
    for (const int n : neighbours)
    {
        if (distances[node] == distances[n] + 1)
        {
            next = n;
            break;
        }
    }
    if (next == -1)
    {
        puts("An error has occurred");
        std::exit(1);
    }
    auto out = get_path(next, distances, indexes, graph);
    out.push_back(node);
    return out;
}