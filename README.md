# Word Ladder Solver
The rules of the game can be found [here](https://en.wikipedia.org/wiki/Word_ladder).

Given a new-line-separated list of 4 letter words, a starting word and an ending word, the programme returns shortest paths from the starting word to the ending one (if such paths exist).
## Installation
### Linux
```bash
g++-11 diameter.cpp solver.cpp -o diameter.out -O3 -fopenmp # requires c++17
g++-11 shortest-paths.cpp solver.cpp -o shortest-paths.out -O3 # requires c++17
```

## Usage
```
$ time ./shortest-paths.out -s hello -e world
Searching for paths from HELLO to WORLD ...
HELLO -> HELLS -> HEALS -> HEALD -> WEALD -> WOALD -> WORLD

real    0m0.390s
user    0m0.362s
sys     0m0.028s
```


```
$ time ./diameter.out -ns 5
ANKHS -> SUHUR(path length:26)                                                            

real    0m39.521s
user    3m51.019s
sys     0m0.381s
```