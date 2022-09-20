#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>

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
	
	printf("%s\n", nom_fichier);
	close(fd);
	return;
}

void *executer (void *s){
	char** requetes = (char **) s;
  	for(;;){
	  	for (int j=0; j<5;j++){
	  		if(requetes[j] != NULL){
	  			estExecutable(requetes[j]);
	  			requetes[j]=0;
	  			break;
	  		}
	  	}
  	}
}

int main(int argc, char *argv[]) {
  
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
		printf("Entrer un nom de fichier :\n");
		scanf("%s",string);

		caseLibre = 0;
		while(caseLibre == 0){
		  	for (int j=0; j<5;j++){
		  		if(requetes[j] == 0){
		  			requetes[j] = string;
		  			caseLibre = 1;
		  			break;
		  		}
		  	}
		}

	}
	return 0;
}




