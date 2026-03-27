/** 
 *  \file
 * \author S. Rubini, UBO 
 * \date février 2026
 *
 * File des travaux prêts à être effectuer par des threads "worker"
 * - Patron de conception "moniteur"
 * - Implantion de la file sous la forme d'un tableau circulaire
 * - Le paramètre d'un travail est un entier 
 * - Les travaux sont gérés par fenêtre glissante : un travail reste
 *   dans la file même si il est terminé tant que tous les travaux
 *   précédants ne sont pas terminés.
 *
 */


#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

#include "travail.h"

/** Création de la file des travaux prêts
 *  \param[in] taille nombre maximum de travaux en attente
 *  \param[in] donnees_applicaton données à diffuser à l'ensemble des threads
 *  \return file des travaux
 */
struct file_travaux  * file_travaux_creer(int taille, void * donnees_application) {
	struct file_travaux * dt;
	int i;

	// allocation de la file
	if ( (dt=(struct file_travaux *)malloc( sizeof(struct file_travaux)) ) == NULL ) {
		return NULL;
	}
	if ( (dt->tampon=(struct travail *)malloc(taille*sizeof(struct travail))) == NULL ) {
		free( dt );
		return NULL;
	}
	
	// initialisation des  semaphores travaux
	for(i=0; i<taille; i++) sem_init(&dt->tampon[i].pret, 0, 0);

	// initialisation des variables de gestion de la file
	dt->premier_occupe=dt->premier_libre=0;
	dt->nb_occupe=0;
	dt->taille=taille;

	// mécanismes de synchronisation du moniteur
	pthread_mutex_init(&dt->mutex_possible, NULL);
	pthread_cond_init(&dt->possible, NULL);

	// pointeur vers une variable permmettant l'acces aux données de l'application
	// a transmettre aux threads 
	dt->donnees=donnees_application;

	return dt;
}

/** Ajout d'un travail à la file
 *  \param[in,out] dt file des travaux
 *  \param[in] valeur paramètre du travail à effectuer 
 */
void file_travaux_ajouter(struct file_travaux *dt, int valeur) {

	pthread_mutex_lock(&dt->mutex_possible);

	while (dt->nb_occupe==dt->taille) {
		// file pleine, on attend que la fin d'un travail 
		pthread_cond_wait(&dt->possible, &dt->mutex_possible);
	}

	dt->tampon[dt->premier_libre].valeur=valeur;
	sem_post( &dt->tampon[dt->premier_libre].pret );

	dt->premier_libre=(dt->premier_libre+1)%dt->taille;
	dt->nb_occupe++;

	assert ( dt->nb_occupe<=dt->taille );

	pthread_mutex_unlock(&dt->mutex_possible);
	return ;
}
	
/** Marquer la fin d'un travail dans la file. Les travaux les plus anciens 
 *  sont supprimés de la file si ils sont terminés, jusqu'à trouver un travail non terminé.
 *  \param[in,out] dt file des travaux
 *  \param[in] valeur paramètre du travail à retirer 
 */
void file_travaux_retirer(struct file_travaux *dt, int pos) {
	pthread_mutex_lock(&dt->mutex_possible);

	dt->tampon[pos].valeur=0; // la valeur 0 indique que le travail est fini

	// liberer la file des travaux finis les plus anciens jusqu'a en trouver un
	//  non terminé
	while( dt->nb_occupe!=0 && dt->tampon[dt->premier_occupe].valeur==0  ) {
		dt->premier_occupe=(dt->premier_occupe+1)%dt->taille;
		dt->nb_occupe--;
		pthread_cond_signal(&dt->possible);
	}

	assert ( dt->nb_occupe>=0 );

	pthread_mutex_unlock(&dt->mutex_possible);
	return; 
}
		
/** Lire le travail le plus ancien stocké dans la file
 *  \param[in,out] dt file des travaux
 *  \return paramètre du travail le plus ancien encore stoké
 */
int file_travaux_lire_dernier(struct file_travaux *dt) {
	int val;
	pthread_mutex_lock(&dt->mutex_possible);
	val=dt->tampon[dt->premier_occupe].valeur;
	pthread_mutex_unlock(&dt->mutex_possible);
	return val;
}
	
	
/** Attendre qu'un travail soit disponible
 *  La fonction n'est pas dans le moniteur. Un seul thread
 *  worker est autorisé à lire un élément dans la tableau qui implante la file.
 *
 *  Le sémaphore associé est différent de 0 uniquement si un travail
 *  est en attente de traitement. La fonction est bloquante en absence de 
 *  travail disponible. 
 *  \param[in,out] dt file des travaux
 *  \param[in] numero numéro du "worker" 
 *  \return paramètre du travail à effectuer 
 */
int file_travaux_lire(struct file_travaux * dt, int numero) {
	sem_wait( &dt->tampon[numero].pret );	
	return dt->tampon[numero].valeur;
}	
	

