#!/bin/bash
#SBATCH --job-name=bishoff1-proj1-8threads
#SBATCH --output output/8threads.out
#SBATCH --error error/8threads.err
#SBATCH --partition=batch
#SBATCH --qos=short
#SBATCH --constraint=hpcf2010
#SBATCH --exclusive
#SBATCH --cpus-per-task=8
#SBATCH --ntasks=1

export KMP_AFFINITY="verbose,compact"
export OMP_NUM_THREADS=8
#echo "10% Coverage size 10"
# ./proj1 ../sample/10coverage/10.txt ../sample/10coverage/targets/t_10.txt
#echo "10% Coverage size 100"
# ./proj1 ../sample/10coverage/100.txt ../sample/10coverage/targets/t_100.txt
#echo "10% Coverage size 1000"
# ./proj1 ../sample/10coverage/1000.txt ../sample/10coverage/targets/t_1000.txt
# echo "10% Coverage size 10000"
# ./proj1 ../sample/10coverage/10000.txt ../sample/10coverage/targets/t_10000.txt
# echo "10% Coverage size 100000"
# ./proj1 ../sample/10coverage/100000.txt ../sample/10coverage/targets/t_100000.txt

echo "20% Coverage size 10"
./proj1 ../sample/20coverage/10.txt ../sample/20coverage/targets/t_10.txt
echo "20% Coverage size 100"
./proj1 ../sample/20coverage/100.txt ../sample/20coverage/targets/t_100.txt
echo "20% Coverage size 1000"
./proj1 ../sample/20coverage/1000.txt ../sample/20coverage/targets/t_1000.txt
# echo "20% Coverage size 10000"
# ./proj1 ../sample/20coverage/10000.txt ../sample/20coverage/targets/t_10000.txt
# echo "20% Coverage size 100000"
# ./proj1 ../sample/20coverage/100000.txt ../sample/20coverage/targets/t_100000.txt

echo "30% Coverage size 10"
./proj1 ../sample/30coverage/10.txt ../sample/30coverage/targets/t_10.txt
echo "30% Coverage size 100"
./proj1 ../sample/30coverage/100.txt ../sample/30coverage/targets/t_100.txt
echo "30% Coverage size 1000"
./proj1 ../sample/30coverage/1000.txt ../sample/30coverage/targets/t_1000.txt
# echo "30% Coverage size 10000"
# ./proj1 ../sample/30coverage/10000.txt ../sample/30coverage/targets/t_10000.txt
# echo "30% Coverage size 100000"
# ./proj1 ../sample/30coverage/100000.txt ../sample/30coverage/targets/t_100000.txt

echo "50% Coverage size 10"
./proj1 ../sample/50coverage/10.txt ../sample/50coverage/targets/t_10.txt
echo "50% Coverage size 100"
./proj1 ../sample/50coverage/100.txt ../sample/50coverage/targets/t_100.txt
echo "50% Coverage size 1000"
./proj1 ../sample/50coverage/1000.txt ../sample/50coverage/targets/t_1000.txt
# echo "50% Coverage size 10000"
# ./proj1 ../sample/50coverage/10000.txt ../sample/50coverage/targets/t_10000.txt
# echo "50% Coverage size 100000"
# ./proj1 ../sample/50coverage/100000.txt ../sample/50coverage/targets/t_100000.txt