# Word Ladder Solver
The rules of the game can be found [here](https://en.wikipedia.org/wiki/Word_ladder).

Given a new-line-separated list of 4 letter words, a starting word and an ending word, the programme returns shortest paths from the starting word to the ending one (if such paths exist).
## Installation
### Linux
```bash
g++-11 diameter.cpp -o diameter.out -O3 -fopenmp
g++-11 shortest-paths.cpp -o shortest-paths.out -O3
```
### Other
It should't be hard: c++17 is required and each .cpp file is compiled separately.

## Usage
```bash
$ time ./shortest-paths.out -s drake -e sings

Searching for paths from DRAKE to SINGS ...
366 words have been visited (2.8214616096207217%)
DRAKE -> BRAKE -> BRAKS -> BOAKS -> BONKS -> BINKS -> BINGS -> SINGS

...

real    0m0.233s
user    0m0.225s
sys     0m0.008s

```


```
$ time ./approx-diameter.out -n 5
Finding the unconnected graphs...
100.00%
Finding approximate diameter of the graph...
100.00%
PATH LENGTH: 27
APIAN -> AVIAN -> AVION -> ANION -> ANCON -> ASCON -> ASCOT -> ASSOT -> ASSET -> ASSES -> APSES -> APRES -> AURES -> SURES -> SERES -> SERRS -> SEARS -> SNARS -> SNARY -> UNARY -> UNARM -> INARM -> INERM -> INERT -> INEPT -> INAPT -> UNAPT


real    1m1.155s
user    1m1.133s
sys     0m0.012s
```