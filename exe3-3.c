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

void *executer(void *s){
	char* nom_fichier = (char*) s;
	estExecutable(nom_fichier);
	return 0;
}


int main(int argc, char *argv[]) {
  signal(SIGUSR1, quefaire);
  printf(" [kill -10 %d] pour arrêter le processus \n", getpid());

  while(1){
  	pthread_t unthread;
  		
  	char* string = malloc(sizeof(char*));
  	printf("Entrer un nom de fichier :\n");
  	scanf("%s",string);
  	
  	pthread_create(&unthread, NULL, executer, string); 
  	pthread_join(unthread, NULL);
  }
  return 0;
}



