
// phase 3 : parallelisation avec workers + file de travaux
// binome : Youssef CHEMANGUI / Abderrahman AMINE
// module systeme 2

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <sched.h>

#include "travail.h"

#define STOP -1

struct donnees_application {
    int *tab;
    int n;
};

struct donnees_worker {
    struct file_travaux *dt;
    int id;
};

void biffer_multiples(struct donnees_application *app, int p) {
    int j;
    for (j = p * p; j < app->n; j += p) {
        app->tab[j] = 0;
    }
}

int file_vide(struct file_travaux *dt) {
    int res;
    pthread_mutex_lock(&dt->mutex_possible);
    res = (dt->nb_occupe == 0);
    pthread_mutex_unlock(&dt->mutex_possible);
    return res;
}

int valeur_certaine(struct file_travaux *dt, int i) {
    int res;

    pthread_mutex_lock(&dt->mutex_possible);

    if (dt->nb_occupe == 0) {
        res = 1;
    } else {
        int p = dt->tampon[dt->premier_occupe].valeur;
        res = (p * p > i);
    }

    pthread_mutex_unlock(&dt->mutex_possible);
    return res;
}

void *worker(void *arg) {
    struct donnees_worker *dw = (struct donnees_worker *)arg;
    struct file_travaux *dt = dw->dt;
    struct donnees_application *app = dt->donnees;
    int id = dw->id;
    int p;

    while (1) {
        p = file_travaux_lire(dt, id);

        if (p == STOP) {
            file_travaux_retirer(dt, id);
            break;
        }

        biffer_multiples(app, p);
        file_travaux_retirer(dt, id);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    int n, T;
    int i;
    int *tab;
    pthread_t *threads;
    struct donnees_worker *args;
    struct donnees_application app;
    struct file_travaux *dt;
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
        printf("erreur d'allocation memoire tab\n");
        return 1;
    }

    threads = (pthread_t *)malloc(T * sizeof(pthread_t));
    if (threads == NULL) {
        printf("erreur d'allocation memoire threads\n");
        free(tab);
        return 1;
    }

    args = (struct donnees_worker *)malloc(T * sizeof(struct donnees_worker));
    if (args == NULL) {
        printf("erreur d'allocation memoire args\n");
        free(tab);
        free(threads);
        return 1;
    }

    app.tab = tab;
    app.n = n;

    dt = file_travaux_creer(T, &app);
    if (dt == NULL) {
        printf("erreur creation file travaux\n");
        free(tab);
        free(threads);
        free(args);
        return 1;
    }

    begin = clock();
    gettimeofday(&debut, NULL);

    tab[0] = 0;
    tab[1] = 0;
    for (i = 2; i < n; i++) {
        tab[i] = 1;
    }

    for (i = 0; i < T; i++) {
        args[i].dt = dt;
        args[i].id = i;
        if (pthread_create(&threads[i], NULL, worker, &args[i]) != 0) {
            printf("erreur creation thread %d\n", i);
            free(dt->tampon);
            free(dt);
            free(tab);
            free(threads);
            free(args);
            return 1;
        }
    }

    for (i = 2; i * i < n; i++) {
        while (!valeur_certaine(dt, i)) {
            sched_yield();
        }

        if (tab[i] == 1) {
            file_travaux_ajouter(dt, i);
        }
    }

    while (!file_vide(dt)) {
        sched_yield();
    }

    for (i = 0; i < T; i++) {
        file_travaux_ajouter(dt, STOP);
    }

    for (i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
    }

    end = clock();
    gettimeofday(&fin, NULL);

    temps_calcul = (double)(end - begin) / CLOCKS_PER_SEC;
    temps_execution = (fin.tv_usec - debut.tv_usec) / 1000000.0
                    + (fin.tv_sec - debut.tv_sec);

    printf("%d,%d,%.6f,%.6f\n", n, T, temps_calcul, temps_execution);

    free(dt->tampon);
    free(dt);
    free(tab);
    free(threads);
    free(args);

    return 0;
}