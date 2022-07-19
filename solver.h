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
        cache[s] = SIZE_MAX;
    return cache;
}

std::list<const std::string *> get_unvisited(const std::vector<const std::string *> &words)
{
    std::list<const std::string *> ret(words.begin(), words.end());
    return ret;
}

inline size_t safe_add(size_t left, size_t right)
{
    return left > SIZE_MAX - right ? SIZE_MAX : left + right;
}

std::unordered_map<const std::string *, size_t> get_distances(const std::string &start, const std::string &end, const std::vector<const std::string *> &words, bool dijkstra)
{
    std::list<const std::string *> unvisited(words.begin(), words.end());
    auto cache = get_cache(words);
    cache[&start] = 0;
    while (unvisited.size() > 0)
    {
        const std::string *min;
        if (dijkstra)
        {
            auto pred = [&cache](const std::string *left, const std::string *right)
            {
                return cache.at(left) < cache.at(right);
            };
            min = *std::min_element(unvisited.begin(), unvisited.end(), pred); // find the minimum unvisited distance to the start
            if (cache.at(min) == SIZE_MAX)
            {
                assert(std::none_of(unvisited.begin(), unvisited.end(), [&cache](const std::string *s)
                                    { return cache.at(s) < SIZE_MAX; }));
                break;
            }
            for (const std::string *s : get_unseen_neighbours(*min, unvisited)) // loop through that word's unvisited neighbours
            {
                cache[s] = std::min(cache.at(s), safe_add(cache.at(min), 1)); // if a shorter path to a neighhbour is found, update it
            }
            unvisited.remove(min); // the minimum has now been visited
        }
        else
        {
            auto pred_approx = [&end, &cache](const std::string *left, const std::string *right)
            {
                size_t left_dist_from_start = cache.at(left);
                size_t right_dist_from_start = cache.at(right);
                size_t left_approx_dist_to_end = hamming_distance(*left, end);
                size_t right_approx_dist_to_end = hamming_distance(*right, end);

                return safe_add(left_approx_dist_to_end, left_dist_from_start) < safe_add(right_approx_dist_to_end, right_dist_from_start);
            };

            min = *std::min_element(unvisited.begin(), unvisited.end(), pred_approx);
            if (min == &end || cache.at(min) == SIZE_MAX)
            {
                break; // don't bother filling the whole graph
            }
            for (const std::string *s : get_unseen_neighbours(*min, unvisited)) // loop through that word's unvisited neighbours
            {
                cache[s] = std::min(cache[s], safe_add(cache[min], 1)); // if a shorter path to a neighhbour is found, update it
            }
            unvisited.remove(min); // the minimum has now been visited
        }
    }
    return cache;
}

std::vector<std::vector<const std::string *>> get_paths(const std::string &node, const std::unordered_map<const std::string *, size_t> &cache, const std::vector<const std::string *> &words)
{
    int my_dist = cache.at(&node);
    if (my_dist == SIZE_MAX)
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
    int my_dist = cache.at(&node);
    if (my_dist == SIZE_MAX)
    {
        std::exit(1);
    }
    else if (my_dist == 0)
    {
        return {&node};
    }
    auto neighbours = get_neighbours(node, words);
    const std::string *next = nullptr;
    for (const std::string *s : get_neighbours(node, words))
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