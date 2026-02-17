echo "n,temp de calcul,Temps d'execution" > resultats.csv
for i in {1..9}
do
    n=$((10**i))
    ./crible $n >> resultats.csv
done
