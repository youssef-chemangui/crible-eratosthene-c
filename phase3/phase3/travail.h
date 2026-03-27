/// \file
#include <pthread.h>
#include <semaphore.h>

// Paramètre d'un travail, et sémaphore de disponibilité
struct travail {
	int valeur;
	sem_t pret;
};

// Données de gestion de la file des travaux, 
// et mécanismes de synchronisation pour la gestion de la file
struct file_travaux {
	int premier_occupe, premier_libre;
	int nb_occupe, taille;
	pthread_mutex_t mutex_possible;
	pthread_cond_t possible;
	struct donnees_application * donnees;
	struct travail  *tampon;
}; 

// fonctions du moniteur travail
struct file_travaux  * file_travaux_creer( int taille, void * donnees );
void file_travaux_ajouter( struct file_travaux *dt, int valeur );
void file_travaux_retirer( struct file_travaux *dt, int pos );
int file_travaux_lire_dernier( struct file_travaux *dt );
		
// fonction hors moniteur
int file_travaux_lire( struct file_travaux *dt, int pos );
	
	
	
	

