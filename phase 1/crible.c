// phase 1 :  travail préparatoire
//binome : Youssef CHEMANGUI / Abderrahman AMINE
// module systeme 2 :
// les ressources utilisées : https://koor.fr/C/ctime/clock.wp pour clock_t et https://man7.org/linux/man-pages/man2/gettimeofday.2.html pour gettimeofday()
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h> 



int main(int argc, char *argv[]) {
    if ( argc != 2){
        printf("vous devez entrer un seul argument \n");
        return 1;
    }
     int n = atoi(argv[1]);
     if (n < 2){
        printf("le nombre doit etre superieur ou egal a 2 \n");
        return 1;
     }
    int *tab = (int *)malloc(n*sizeof(int));
    struct timeval debut, fin;
    clock_t begin = clock();
    gettimeofday(&debut, NULL);
    
    tab[0]=0;
    tab[1]=0;

    for(int i = 2; i<n;i++){
        tab[i] = 1;
    }
    for(int i = 2; i*i<n;i++){
        if (tab[i] == 1){
            for (int j = i*i ; j<n ;j+=i){
                tab[j] = 0;
            }
        }
    }
    
    /*printf("les nombres premiers : ");
    for(int i = 2; i<n;i++){
        if (tab[i] == 1){
            printf("%d ", i);
            }
    }*/


    clock_t end = clock();
    gettimeofday(&fin, NULL);
    double temps_calcul = (double) (end - begin)   /  CLOCKS_PER_SEC;
    double temps_execution = (fin.tv_usec - debut.tv_usec)/1000000.0 + (fin.tv_sec - debut.tv_sec);
    printf("%d,%.6f,%.6f\n", n, temps_calcul, temps_execution);



    printf("\n");
    free(tab);
    return 0;

   

}