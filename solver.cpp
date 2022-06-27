#include "solver.h"
#include <chrono>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <iomanip>
std::string to_upper(std::string input)
{
    transform(input.begin(), input.end(), input.begin(), ::toupper);
    return input;
}

std::vector<std::string> load_words(const std::string &filename)
{
    std::ifstream file(filename, std::ifstream::in);
    if (!file.is_open())
    {
        printf("File doesn't exist");
    }
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line))
    {
        assert(line.size() == 4);
        lines.push_back(to_upper(line));
    }
    return lines;
}

size_t hamming_distance(const std::string &s1, const std::string &s2)
{
    size_t dist = 0;
    for (size_t i = 0; i < 4; i++)
        dist += (s1[i] == s2[i] ? 0 : 1);
    return dist;
}
std::vector<const std::string *> get_neighbours(const std::string &node)
{
    std::vector<const std::string *> output;
    for (const auto &other : words)
    {
        if (hamming_distance(node, other) == 1)
            output.push_back(&other);
    }
    return output;
}

std::unordered_map<const std::string *, size_t> get_cache()
{
    std::unordered_map<const std::string *, size_t> cache;
    cache.clear();
    for (const std::string &s : words)
        cache[&s] = SIZE_MAX;
    return cache;
}

auto pred = [](const std::string *left, const std::string *right)
{
    return cache[left] < cache[right];
};

void get_distances(const std::string &start, const std::string &end)
{
    cache = get_cache();
    cache[&start] = 0;
    size_t prev_count = SIZE_MAX;
    auto pred_approx = [end](const std::string *left, const std::string *right)
    {
        size_t left_approx = hamming_distance(*left, end);
        size_t right_approx = hamming_distance(*right, end);

        size_t left_value = cache[left] < SIZE_MAX ? cache[left] + left_approx : SIZE_MAX;
        size_t right_value = cache[right] < SIZE_MAX ? cache[right] + right_approx : SIZE_MAX;
        return left_value < right_value;
    };
    while (unvisited.size() != prev_count && unvisited.size() > 0)
    {
        prev_count = unvisited.size();
        const std::string *min;
        if (dijkstra)
            min = *std::min_element(unvisited.begin(), unvisited.end(), pred);
        else
        {
            min = *std::min_element(unvisited.begin(), unvisited.end(), pred_approx);
            if (min == &end)
            {
                break;
            }
        }
        for (const std::string *s : get_neighbours(*min))
        {
            size_t min_dist = cache[min];
            cache[s] = std::min(cache[s], cache[min] == SIZE_MAX ? cache[min] : min_dist + 1);
        }
        unvisited.remove(*std::min_element(unvisited.begin(), unvisited.end(), pred));
    }
}

void make_tree(const std::string &start, node_t &node)
{
    if (cache[node.value] == SIZE_MAX)
    {
        return;
    }
    if (node.value == &start)
    {
        return;
    }

    auto neighbours = get_neighbours(*node.value);
    auto min = std::min_element(neighbours.begin(), neighbours.end(), pred);
    for (auto i = neighbours.begin(), toofar = neighbours.end(); i != toofar; ++i)
        if (cache[*i] == cache[*min])
        {
            node.children.push_back({{}, *i});
            make_tree(start, node.children.back());
        }
}

void print_tree(node_t root, int depth)
{
    if (cache[root.value] == SIZE_MAX)
    {
        std::cout << "No path exists between the words" << std::endl;
        return;
    }
    if (root.children.size() == 0)
    {
        std::cout << *root.value;
        return;
    }
    for (auto child : root.children)
    {
        std::cout << *root.value << " -> ";
        print_tree(child, depth + 1);
        if (root.children.size() > 1)
            std::cout << std::endl;
        std::cout << std::string(8 * depth, ' ');
    }
}
