#!/bin/bash

gcc -fopenmp proj1.c -o slurm/proj1
cd slurm
bash proj1-1thread.slurm
bash proj1-2threads.slurm
bash proj1-4threads.slurm
bash proj1-8threads.slurm