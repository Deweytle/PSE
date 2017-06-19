#include "pse.h"

#define    CMD            "serveur"
#define    NTHREADS       2
#define    MILLISECONDES  1000
#define    ATTENTE        2000*MILLISECONDES

/* ferme le fichier, le rouvre vide et retourne le nouveu descripteur */
int remiseAZeroLog(int fd, int mode);
int journal;
sem_t jetons;

void *traiterRequete(void *arg) {
  DataSpec * data = (DataSpec *) arg;
  int arret = FAUX, nblus, mode;
  char texte[LIGNE_MAX];

  mode = O_WRONLY|O_APPEND|O_CREAT|O_TRUNC;

  while (VRAI) {
    printf("worker %d: attente canal.\n", data->tid);
    /* attente canal */
    if(sem_wait(&data->sem) == -1){
      perror("wait semaphore");
    }

    data->libre = FAUX;
    printf("worker %d: lecture canal %d.\n", data->tid, data->canal);

    arret = FAUX;
    while (arret == FAUX) {
      nblus = lireLigne (data->canal, texte);
      if (nblus <= 0 || nblus == LIGNE_MAX) {
        erreur("lireLigne\n");
      }
      if (strcmp(texte, "fin") == 0) {
	      printf("worker %d: deconnexion demandee.\n", data->tid);
	      arret = VRAI;
      }
      else if (strcmp(texte, "init") == 0) {
	      printf("worker %d: remise a zero du journal demandee.\n", data->tid);
        journal = remiseAZeroLog(journal, mode);
	    }
      else if (ecrireLigne(journal, texte) != -1) {
	      printf("worker %d: ligne de %d octets ecrite dans le journal.\n",
	              data->tid, nblus);
      }
      else {
        erreur_IO("ecrireLigne");
      }
    }

    if (close(data->canal) == -1) {
      erreur_IO("close");
    }
    data->canal = -1;
    data->libre = VRAI;
    if(sem_post(&jetons) == -1){
        perror("Post Jetons");
    }
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  int ecoute, canal, ret, mode, ilibre, i;
  struct sockaddr_in adrEcoute, reception;
  socklen_t receptionlen = sizeof(reception);
  short port;

  remplir_login_array(login_array);

  DataSpec cohorte[NTHREADS];

  if(sem_init(&jetons,0,NTHREADS) == -1){
      perror("init jetons");
  }

  if (argc != 2) {
    erreur("usage: %s port\n", argv[0]);
  }

  mode = O_WRONLY|O_APPEND|O_CREAT;
  journal = open("journal.log", mode, 0660);
  if (journal == -1) {
    erreur_IO("open journal");
  }

  /* initialisation cohorte */
  for (i=0; i<NTHREADS; i++) {
    cohorte[i].tid = i;
    cohorte[i].libre = VRAI;
    /* une valeur -1 indique pas de requete a traiter */
    cohorte[i].canal = -1;
    if(sem_init(&cohorte[i].sem,0,0) == -1){
      perror("init sem");
    }
    ret = pthread_create(&cohorte[i].id, NULL, traiterRequete, &cohorte[i]);
    if (ret != 0) {
      erreur_IO("pthread_create");
    }
  }

  port = (short) atoi(argv[1]);

  printf("%s: creating a socket\n", CMD);
  ecoute = socket (AF_INET, SOCK_STREAM, 0);
  if (ecoute < 0) {
    erreur_IO("socket");
  }

  adrEcoute.sin_family = AF_INET;
  adrEcoute.sin_addr.s_addr = INADDR_ANY;
  adrEcoute.sin_port = htons(port);
  printf("%s: binding to INADDR_ANY address on port %d\n", CMD, port);
  ret = bind (ecoute,  (struct sockaddr *) &adrEcoute, sizeof(adrEcoute));
  if (ret < 0) {
    erreur_IO("bind");
  }

  printf("%s: listening to socket\n", CMD);
  ret = listen (ecoute, 20);
  if (ret < 0) {
    erreur_IO("listen");
  }

  while (VRAI) {

    printf("%s: waiting to a connection\n", CMD);
    canal = accept(ecoute, (struct sockaddr *) &reception, &receptionlen);
    if(sem_wait(&jetons) == -1){
      perror("wait jeton");
    }

    if (canal < 0) {
      erreur_IO("accept");
    }
    printf("%s: adr %s, port %hu\n", CMD,
    stringIP(ntohl(reception.sin_addr.s_addr)), ntohs(reception.sin_port));


    for ( ilibre = 0; ilibre < NTHREADS; ilibre++) {
      if(cohorte[ilibre].libre) break;
    }
    printf("serveur: %d\n", ilibre);

    cohorte[ilibre].canal = canal;
    if(sem_post(&cohorte[ilibre].sem) == -1){
      perror("post sem");
    }


    printf("%s: worker %d choisi\n", CMD, ilibre);
  }

  exit(EXIT_SUCCESS);
}

int remiseAZeroLog(int fd, int mode) {
  int newFd;
  if (close(fd) == -1) {
    erreur_IO("close log");
  }
  newFd = open("journal.log", mode|O_TRUNC, 0660);
  if (newFd == -1) {
    erreur_IO("open trunc log");
  }
  return newFd;
}
