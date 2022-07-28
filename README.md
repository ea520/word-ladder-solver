# Word Ladder Solver
The rules of the game can be found [here](https://en.wikipedia.org/wiki/Word_ladder).

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
$ time ./diameter.out -ns 2 3 4 5 6 7 8 9 10 11 12 13 14 15
BA -> UP(path length:4)                                                                   
EXO -> ZZZ(path length:8)                                                                 
UNAU -> YEOW(path length:15)                                                              
ANKHS -> SUHUR(path length:26)                                                            
ANEATH -> EMBOIL(path length:42)                                                          
HAGFISH -> INJECTS(path length:64)                                                        
TOWNLING -> TWIDDLER(path length:79)                                                      
BREEDINGS -> SCANNINGS(path length:33)                                                    
BLISTERING -> SNOTTERING(path length:10)                                                  
DODGINESSES -> NERVINESSES(path length:26)                                                
MATERIALIZED -> PATERNALISTS(path length:6)                                               
FACTIONALISMS -> FICTIONALIZED(path length:4)                                             
ROOFLESSNESSES -> WORKLESSNESSES(path length:6)                                           
EXPANDABILITIES -> EXTENSIBILITIES(path length:4)                                         

real	7m1.962s
user	19m50.405s
sys	0m1.387s

```