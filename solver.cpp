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
        if (line.size() == word_length)
            lines.push_back(to_upper(line));
    }
    return lines;
}

size_t hamming_distance(const std::string &s1, const std::string &s2)
{
    size_t dist = 0;
    for (size_t i = 0; i < word_length; i++)
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

std::vector<const std::string *> get_unseen_neighbours(const std::string &node)
{
    std::vector<const std::string *> output;
    for (const auto &other : unvisited)
    {
        if (hamming_distance(node, *other) == 1)
            output.push_back(other);
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

bool pred(const std::string *left, const std::string *right)
{
    return cache[left] < cache[right];
};

inline size_t safe_add(size_t left, size_t right)
{
    return left > SIZE_MAX - right ? SIZE_MAX : left + right;
}

void get_distances(const std::string &start, const std::string &end)
{
    cache = get_cache();
    cache[&start] = 0;
    auto pred_approx = [end](const std::string *left, const std::string *right)
    {
        size_t left_dist_from_start = cache[left];
        size_t right_dist_from_start = cache[right];
        size_t left_approx_dist_to_end = hamming_distance(*left, end);
        size_t right_approx_dist_to_end = hamming_distance(*right, end);

        return safe_add(left_approx_dist_to_end, left_dist_from_start) < safe_add(right_approx_dist_to_end, right_dist_from_start);
    };

    while (unvisited.size() > 0)
    {
        const std::string *min;
        if (dijkstra)
        {
            min = *std::min_element(unvisited.begin(), unvisited.end(), pred); // find the minimum unvisited distance to the start
            for (const std::string *s : get_unseen_neighbours(*min))           // loop through that word's unvisited neighbours
            {
                cache[s] = std::min(cache[s], safe_add(cache[min], 1)); // if a shorter path to a neighhbour is found, update it
            }
            unvisited.remove(min); // the minimum has now been visited
        }
        else
        {
            min = *std::min_element(unvisited.begin(), unvisited.end(), pred_approx);
            if (min == &end)
            {
                break; // don't bother filling the whole graph
            }
            for (const std::string *s : get_unseen_neighbours(*min)) // loop through that word's unvisited neighbours
            {
                cache[s] = std::min(cache[s], safe_add(cache[min], 1)); // if a shorter path to a neighhbour is found, update it
            }
            unvisited.remove(min); // the minimum has now been visited
        }
    }
}

std::vector<std::vector<const std::string *>> get_paths(const std::string &node)
{
    int my_dist = cache[&node];
    if (my_dist == SIZE_MAX)
    {
        std::exit(1);
    }
    else if (my_dist == 0)
    {
        return {{&node}};
    }
    auto neighbours = get_neighbours(node);
    std::vector<std::vector<const std::string *>> out;
    for (auto i = neighbours.begin(), toofar = neighbours.end(); i != toofar; ++i)
        if (cache[*i] == cache[&node] - 1)
        {
            std::vector<std::vector<const std::string *>> new_paths = get_paths(**i.base());
            for (auto &path : new_paths)
            {
                path.push_back(&node);
                out.push_back(path);
            }
        }
    return out;
}