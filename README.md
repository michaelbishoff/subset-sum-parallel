# Parallel Subset Sum Solution
Finds a solution to the [Subset sum problem](https://en.wikipedia.org/wiki/Subset_sum_problem) with OpenMP's multithreading library. Program is designed to run on the [UMBC High Performance Computing Facility](http://hpcf.umbc.edu/) cluster, but I adapted the scripts so that it can run locally.


### Run a single test
```
gcc-5 proj1.c -fopenmp -o proj1
./proj1 sample/50coverage/10.txt sample/50coverage/targets/t_10.txt
```

### Run all tests
```
./run_all.sh
```

### Note
OpenMP 2.5 exists in gcc >=4.2  
OpenMP 3.0 exists in gcc >=4.4  
OpenMP 4.0 exists in gcc >=4.9