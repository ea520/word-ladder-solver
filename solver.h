#include <vector>
#include <string>
#include <unordered_map>
#include <list>
#include <chrono>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits.h>

std::string to_upper(std::string input)
{
    transform(input.begin(), input.end(), input.begin(), ::toupper);
    return input;
}

std::vector<const std::string *> load_words(const std::string &filename, size_t word_length)
{
    static std::vector<std::string> _words;
    if (_words.size() == 0)
    {
        std::ifstream file(filename, std::ifstream::in);
        if (!file.is_open())
        {
            printf("File doesn't exist\n");
        }
        std::string line;
        while (std::getline(file, line))
        {
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
            if (line.size() == word_length)
                _words.push_back(to_upper(line));
        }
    }
    std::vector<const std::string *> ret;
    for (const std::string &word : _words)
        ret.push_back(&word);
    return ret;
}

size_t hamming_distance(const std::string &s1, const std::string &s2)
{
    size_t dist = 0;
    for (size_t i = 0; i < s1.size(); i++)
        dist += (s1[i] == s2[i] ? 0 : 1);
    return dist;
}

std::vector<const std::string *> get_neighbours(const std::string &node, const std::vector<const std::string *> &words)
{
    std::vector<const std::string *> output;
    for (const auto &other : words)
    {
        if (hamming_distance(node, *other) == 1)
            output.push_back(other);
    }
    return output;
}

std::vector<const std::string *> get_unseen_neighbours(const std::string &node, const std::list<const std::string *> &unvisited)
{
    std::vector<const std::string *> output;
    for (const auto &other : unvisited)
    {
        if (hamming_distance(node, *other) == 1)
            output.push_back(other);
    }
    return output;
}

std::unordered_map<const std::string *, size_t> get_cache(const std::vector<const std::string *> &words)
{
    std::unordered_map<const std::string *, size_t> cache;
    cache.clear();
    for (const std::string *s : words)
        cache[s] = INT_MAX;
    return cache;
}

std::list<const std::string *> get_unvisited(const std::vector<const std::string *> &words)
{
    std::list<const std::string *> ret(words.begin(), words.end());
    return ret;
}

std::unordered_map<const std::string *, size_t> get_distances(const std::string *start, const std::string *end, const std::vector<const std::string *> &words, bool dijkstra)
{
    std::list<const std::string *> unvisited(words.begin(), words.end());
    std::unordered_map<const std::string *, std::pair<size_t, size_t>> hammings;
    hammings.reserve(words.size());
    for (const std::string *name : words)
    {
        hammings[name] = {INT_MAX, dijkstra ? 0 : hamming_distance(*name, *end)};
    }
    hammings[start].first = 0;

    auto pred = [&hammings](const std::string *left, const std::string *right) -> bool
    {
        return hammings.at(left).first + hammings.at(left).second < hammings.at(right).first + hammings.at(right).second;
    };
    while (unvisited.size() > 0)
    {
        const std::string *min;
        min = *std::min_element(unvisited.begin(), unvisited.end(), pred); // find the minimum unvisited distance to the start
        if (hammings.at(min).first == INT_MAX || min == end)
        {
            break;
        }
        for (const std::string *s : get_unseen_neighbours(*min, unvisited)) // loop through that word's unvisited neighbours
        {
            hammings[s].first = std::min(hammings.at(s).first, hammings.at(min).first + 1); // if a shorter path to a neighhbour is found, update it
        }
        unvisited.remove(min); // the minimum has now been visited
    }
    std::unordered_map<const std::string *, size_t> cache;
    cache.reserve(hammings.size());
    for (const std::string *name : words)
    {
        cache[name] = hammings[name].first;
    }

    return cache;
}

std::vector<std::vector<const std::string *>> get_paths(const std::string &node, const std::unordered_map<const std::string *, size_t> &cache, const std::vector<const std::string *> &words)
{
    size_t my_dist = cache.at(&node);
    if (my_dist == INT_MAX)
    {
        std::exit(1);
    }
    else if (my_dist == 0)
    {
        return {{&node}};
    }
    auto neighbours = get_neighbours(node, words);
    std::vector<std::vector<const std::string *>> out;
    for (auto i = neighbours.begin(), toofar = neighbours.end(); i != toofar; ++i)
        if (cache.at(*i) == cache.at(&node) - 1)
        {
            std::vector<std::vector<const std::string *>> new_paths = get_paths(**i, cache, words);
            for (auto &path : new_paths)
            {
                path.push_back(&node);
                out.push_back(path);
            }
        }
    return out;
}

std::vector<const std::string *> get_path(const std::string &node, const std::unordered_map<const std::string *, size_t> &cache, const std::vector<const std::string *> &words)
{
    size_t my_dist = cache.at(&node);
    if (my_dist == INT_MAX)
    {
        std::exit(1);
    }
    else if (my_dist == 0)
    {
        return {&node};
    }
    auto neighbours = get_neighbours(node, words);
    const std::string *next = nullptr;
    for (const std::string *s : neighbours)
    {
        if (cache.at(&node) - 1 == cache.at(s))
        {
            next = s;
            break;
        }
    }
    if (next == nullptr)
    {
        std::cout << "UNKNOWN ERROR" << std::endl;
    }
    auto out = get_path(*next, cache, words);
    out.push_back(&node);
    return out;
}