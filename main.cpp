#include "solver.h"
#include "argparse.h"
#include <chrono>
std::vector<std::string> words;
std::list<const std::string *> unvisited;
std::unordered_map<const std::string *, size_t> cache;
bool dijkstra = true;
int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("weaver");
    program.add_argument("-l", "--word-list").help("Path to the list of keywords").default_value(std::string("word-list.txt"));

    program.add_argument("-s", "--start").help("The starting word in the game (must be 4 letters)").required().default_value(std::string("SLOW"));

    program.add_argument("-e", "--end").help("The ending word in the game (must be 4 letters)").required().default_value(std::string("FAST"));

    program.add_argument("-d", "--dijkstra").help("Whether to use Dijkstra (slower but gives all solutions) or A*").default_value(true).implicit_value(true);
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
    
    std::string filename = program.get<std::string>("-l");
    dijkstra = program.get<bool>("-d");
    words = load_words(filename);
    
    // swap end and start as this implementation makes it easier to print from end to start
    const std::string *start = &*std::find(words.begin(), words.end(), to_upper(program.get<std::string>("-e")));
    const std::string *end = &*std::find(words.begin(), words.end(), to_upper(program.get<std::string>("-s")));
    
    if (end == &*words.end())
    {
        std::cerr << "The starting word (" << to_upper(program.get<std::string>("-s")) << ") is not in the word list\n"; // again end and start are swapped
        return -1;
    }
    
    if (start == &*words.end())
    {
        std::cerr << "The ending word (" << to_upper(program.get<std::string>("-e")) << ") is not in the word list\n";
        return -1;
    }
    
    std::cerr << *end + " -> " + *start << std::endl;

    for (const std::string &word : words)
    {
        unvisited.push_back(&word);
    }
    
    auto t0 = std::chrono::high_resolution_clock::now();
    get_distances(*start, *end);
    auto t1 = std::chrono::high_resolution_clock::now();
    node_t root = {{}, end};
    make_tree(*start, root);
    std::cerr << "Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << "ms" << std::endl;
    size_t visited_count = std::count_if(cache.begin(), cache.end(), [](std::pair<const std::string *, size_t> s)
                                         { return s.second != SIZE_MAX; });
    std::cerr << "Number of nodes seen " << visited_count << std::endl;
    print_tree(root);
}
