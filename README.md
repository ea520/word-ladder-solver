# Word Ladde Solver
The rules of the game can be found [here](https://en.wikipedia.org/wiki/Word_ladder).

Given a new-line-separated list of 4 letter words, a starting word and an ending word, the programme returns shortest paths from the starting word to the ending one (if such paths exist).
## Installation
### Linux
```bash
g++-11 main.cpp weaver.cpp -O3 -oweaver --std=c++17
```
### Other
It should't be hard: c++17 is required and there are just 2 cpp files and no external libraries

## Usage
```bash
./weaver -l word-list.txt -start PASS -end Scab --dijkstra # it's case insensitive for start and end
```
## Example output
```
PASS -> SCAB
Duration: 721ms
Number of nodes seen 5563
PASS -> PANS -> PANG -> SANG -> SAAG -> SCAG -> SCAB                                                                                                                
        PANS -> SANS -> SANG -> SAAG -> SCAG -> SCAB                                                                                                                
        
PASS -> PAST -> PEST -> PEAT -> SEAT -> SCAT -> SCAB                                                                                                                        
PASS -> SASS -> SANS -> SANG -> SAAG -> SCAG -> SCAB                                                                                                                
        SASS -> SESS -> SEAS -> SEAM -> SCAM -> SCAB                                                                        
                        SEAS -> SEAN -> SCAN -> SCAB                                                                        
                        SEAS -> SEAR -> SCAR -> SCAB                                                                        
                        SEAS -> SEAT -> SCAT -> SCAB    
```
