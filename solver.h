#pragma once
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <algorithm>
std::vector<int32_t> get_distances_bfs(const int32_t start, const std::vector<int32_t> &indexes, const std::vector<std::vector<int32_t>> &graph);
std::vector<int32_t> get_distances_bfs(const int32_t start, const int32_t end, const std::vector<int32_t> &indexes, const std::vector<std::vector<int32_t>> &graph);
std::vector<int32_t> get_distances_dijkstra(const int32_t start, const std::vector<int32_t> &indexes, const std::vector<std::vector<int32_t>> &graph);
std::vector<int32_t> get_distances_dijkstra(const int32_t start, const int32_t end, const std::vector<int32_t> &indexes, const std::vector<std::vector<int32_t>> &graph);
std::vector<int32_t> get_path(const int node, const std::vector<int32_t> &distances, const std::vector<int32_t> &indexes, const std::vector<std::vector<int32_t>> &graph);
std::string to_upper(std::string input);
