#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>


typedef struct{
	sem_t S1; //Un seul thread accepte la requete
	sem_t S2; //Acces au tableau partagé
	sem_t S3; //Le main attend la fin du travail du thread qui a accepté la requete pour redemander un mot
} Controle;

Controle controle;

/*Initialisation des sémaphores*/ 
void initialisecontrole(Controle *res){
	sem_init( &(res->S1), 0, 0); // 1 = requete en attente -> 0 = requete accepté 
	sem_init( &(res->S2), 0, 1); // 1 = le tableau est accessible -> 0 = le tableau est en cours d'utilisation
	sem_init( &(res->S3), 0, 1); //1 = pas de travail en cours -> 0 = un travail est encore en cours
} 

/*Un seul thread accepte la requete*/
void accepterRequete(Controle *res){
	sem_wait(&res->S1) ; 
} 

void ajouterRequete(Controle *res){
	sem_post(&res->S1) ; 
} 

/* Acces au tableau partagé*/
void debutUtiliser(Controle *res){
	sem_wait(&res->S2) ; 
} 

void finUtiliser(Controle *res){
	sem_post(&res->S2) ; 
} 

/*Le main attend la fin du travail du thread qui a accepté la requete pour redemander un mot*/
void FinTravail(Controle *res){
	sem_post(&res->S3) ; 
} 

void attenteFinTravail(Controle *res){
	sem_wait(&res->S3) ; 
} 

/*Signal*/
void quefaire(int sig){
	if(sig == SIGUSR1){
	printf("Fin du programme\n");
	exit(1);
	}
}

void estExecutable(char *nom_fichier) {
	printf("ID du thread: %d\n", gettid());

	int tab[4]={127,69,76,70}; //caracteristiques d'un fichier executable
	
	int fd;
	if( (fd = open(nom_fichier, O_RDONLY) ) == -1) {
	    perror("erreur d'ouverture du fichier");
	    return;
	}

	char buffer[10];
	read(fd, buffer, 4); //on lit les 4 premiers octets du fichier
	
	for(int i=0; i < 4; i++){
	    if(buffer[i] != tab[i]){
	    	close(fd);
	  	return;
	    }
	}
	
	printf("%s est un fichier executable\n", nom_fichier);
	close(fd);
	return;
}

void *executer (void *s){
	char** requetes = (char **) s;
  	
  	for(;;){
		accepterRequete(&controle);
		printf("Requete acceptée par un thread.\n");

	  	for (int j=0; j<5;j++){
	  	
	  		printf("thread : j'utilise le tableau de requetes \n");
	  		debutUtiliser(&controle); 
	  		
	  		if(requetes[j] != NULL){         
	  			estExecutable(requetes[j]);  
	  			requetes[j]=0; 

	  			finUtiliser(&controle);
	  			printf("thread : je n'utilise plus le tableau de requetes \n\n");
	  			
	  			FinTravail(&controle);
	  			break;
	  		}
	  	}
  	}
}

int main(int argc, char *argv[]) {

	initialisecontrole(&controle);          	                                                

	signal(SIGUSR1, quefaire);
	printf(" [kill -10 %d] pour arrêter le processus \n", getpid());

	char *requetes[5] = {0,0,0,0,0};
	pthread_t tab_thread[10];

	for(int i=0; i<10;i++){
		pthread_create(&(tab_thread[i]), NULL, executer, requetes);
	}

	char* string = malloc(sizeof(char*));
	int caseLibre;

	while(1){
		attenteFinTravail(&controle);

		printf("Entrer un nom de fichier :\n");
		scanf("%s",string);

		caseLibre = 0;
		while(caseLibre == 0){
		  	for (int j=0; j<5;j++){
		  	
		  		printf("main : j'utilise le tableau de requetes \n");
		  		debutUtiliser(&controle);
		  		
		  		if(requetes[j] == 0){  
		  			requetes[j] = string; 
		  			
		  			finUtiliser(&controle);
					printf("main : je n'utilise plus le tableau de requetes \n");
					
					printf("main : Informe aux threads de l'ajout d'une requete %s\n", string);
		  			ajouterRequete(&controle); 
		  			
		  			caseLibre = 1;
		  			break;
		  		}
		  	}
		}
	}
  
}


