#!/bin/bash

echo "Nombre de threads,temps_calcul,temps_execution" > resultats2.csv

N=1000000000

for i in $(seq 0 10)
do
    T=$((2**i))
    ./crible2 $N $T >> resultats2.csv
done