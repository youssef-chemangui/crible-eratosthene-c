// phase 2 : parallelisation avec threads
// binome : Youssef CHEMANGUI / Abderrahman AMINE
// module systeme 2
// les ressources utilisées : https://koor.fr/C/ctime/clock.wp pour clock_t et https://man7.org/linux/man-pages/man2/gettimeofday.2.html pour gettimeofday()

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>


int *tab;
int n_global;

struct donnees_thread {
    pthread_t tid;
    int premier;   
    int alive;    
};


void *biffer_multiples(void *param) {
    struct donnees_thread *d = (struct donnees_thread *)param;
    int p = d->premier;
    int j;

    for (j = p * p; j < n_global; j += p) {
        tab[j] = 0;
    }

    return NULL;
}


int main(int argc, char *argv[]) {
    int n, T;
    int i, j;
    struct timeval debut, fin;
    clock_t begin, end;
    double temps_calcul, temps_execution;

    if (argc != 3) {
        printf("usage : %s N T\n", argv[0]);
        return 1;
    }

    n = atoi(argv[1]);
    T = atoi(argv[2]);

    if (n < 2) {
        printf("N doit etre superieur ou egal a 2\n");
        return 1;
    }
    if (T < 1) {
        printf("T doit etre superieur ou egal a 1\n");
        return 1;
    }

    tab = (int *)malloc(n * sizeof(int));
    if (tab == NULL) {
        printf("erreur d'allocation memoire\n");
        return 1;
    }

    n_global = n;

    begin = clock();
    gettimeofday(&debut, NULL);

    tab[0] = 0;
    tab[1] = 0;
    for (i = 2; i < n; i++) {
        tab[i] = 1;
    }



    i = 2; 

    while (i * i < n) {

        struct donnees_thread *groupe;
        int taille_groupe = 0;
        int k = i;
        int nb = 0;
        while (k * k < n && nb < T) {
            if (tab[k] == 1) {
                nb++;
            }
            k++;
        }
        taille_groupe = nb;

        if (taille_groupe == 0)
            break;

        groupe = (struct donnees_thread *)malloc(taille_groupe * sizeof(struct donnees_thread));
        if (groupe == NULL) {
            printf("erreur d'allocation pour le groupe de threads\n");
            free(tab);
            return 1;
        }

        k = i;
        nb = 0;
        while (k * k < n && nb < taille_groupe) {
            if (tab[k] == 1) {
                groupe[nb].premier = k;
                groupe[nb].alive = 0;

                if (pthread_create(&groupe[nb].tid, NULL,
                                   &biffer_multiples, &groupe[nb]) == 0) {
                    groupe[nb].alive = 1;
                } else {
                    for (j = k * k; j < n; j += k) {
                        tab[j] = 0;
                    }
                }
                nb++;
            }
            k++;
        }

        for (nb = 0; nb < taille_groupe; nb++) {
            if (groupe[nb].alive) {
                pthread_join(groupe[nb].tid, NULL);
            }
        }

        while (k < n && tab[k] != 1) {
            k++;
        }
        i = k;

        free(groupe);
    }

    end = clock();
    gettimeofday(&fin, NULL);

    temps_calcul   = (double)(end - begin) / CLOCKS_PER_SEC;
    temps_execution = (fin.tv_usec - debut.tv_usec) / 1000000.0 + (fin.tv_sec  - debut.tv_sec);

    printf("%d,%.6f,%.6f\n", T, temps_calcul, temps_execution);

    free(tab);
    return 0;
}