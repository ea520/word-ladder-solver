#include <vector>
#include <string>
#include <unordered_map>
#include <list>

std::vector<std::string> load_words(const std::string &filename);
void get_distances(const std::string &, const std::string &);
std::string to_upper(std::string input);

struct node_t
{
    std::vector<node_t> children{};
    const std::string *value{};
};

void make_tree(const std::string &start, node_t &node);
void print_tree(node_t root, int depth = 0);

extern std::vector<std::string> words;
extern std::list<const std::string *> unvisited;
extern std::unordered_map<const std::string *, size_t> cache;
extern bool dijkstra;
