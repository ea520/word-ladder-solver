# Word Ladder Solver
The rules of the game can be found [here](https://en.wikipedia.org/wiki/Word_ladder).

Given a new-line-separated list of 4 letter words, a starting word and an ending word, the programme returns shortest paths from the starting word to the ending one (if such paths exist).
## Installation
### Linux
```bash
g++-11 main.cpp solver.cpp -O3 -osolver.out --std=c++17
```
### Other
It should't be hard: c++17 is required and there are just 2 cpp files and no external libraries

## Usage
```bash
time ./solver.out -s played -e creeps --dijkstra
```
## Output
```
PLAYED -> PLACED -> PLACES -> PLACKS -> CLACKS -> CLECKS -> CLEEKS -> CLEEPS -> CREEPS
PLAYED -> PLACED -> PLACES -> PLACKS -> CLACKS -> CLECKS -> CLEEKS -> CREEKS -> CREEPS

real    0m8.934s
user    0m8.923s
sys     0m0.008s
```

```bash
time ./solver.out -s played -e creeps
```
```
PLAYED -> PLACED -> PLACES -> PLACKS -> CLACKS -> CLECKS -> CLEEKS -> CLEEPS -> CREEPS
PLAYED -> PLACED -> PLACES -> PLACKS -> CLACKS -> CLECKS -> CLEEKS -> CREEKS -> CREEPS

real    0m0.292s
user    0m0.284s
sys     0m0.008s
```
