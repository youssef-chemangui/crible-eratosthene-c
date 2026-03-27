#!/bin/bash

echo "N,T,temps_calcul,temps_execution" > resultats3.csv

N=1000000000

for i in {0..10}
do
    T=$((2**i))
    ./crible3 $N $T >> resultats3.csv
done