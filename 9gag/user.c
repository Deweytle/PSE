#include "pse.h"

#define CMD   "client"

int main(int argc, char *argv[]) {
  int sock, arret = VRAI, ret, nbecr, auth_success = 0;
  struct sockaddr_in *sa;
  char texte[LIGNE_MAX];
  char user[20];

  if (argc != 3) {
    erreur("usage: %s machine port\n", argv[0]);
  }

  printf("%s: creating a socket\n", CMD);
  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    erreur_IO("socket");
  }

  printf("%s: DNS resolving for %s, port %s\n", CMD, argv[1], argv[2]);
  sa = resolv(argv[1], argv[2]);
  if (sa == NULL) {
    erreur("adresse %s et port %s inconnus\n", argv[0], argv[1]);
  }
  printf("%s: adr %s, port %hu\n", CMD,
        stringIP(ntohl(sa->sin_addr.s_addr)), ntohs(sa->sin_port));

  /* connexion sur site distant */
  printf("%s: connecting the socket\n", CMD);
  ret = connect(sock, (struct sockaddr *) sa, sizeof(struct sockaddr_in));
  if (ret < 0) {
    erreur_IO("connect");
  }

  /* Logo de connection au serveur */
  printf("\n");
  printf("\n");
  printf("                                               -oddo-\n");
  printf("                                           `/yNMMMMMMNy/`\n");
  printf("                                        -odMMMMMMMMMMMMMMdo-\n");
  printf("                                    `/yNMMMMMMMMMMMMMMMMMMMMNy/`\n");
  printf("                                 -odMMMMMMMMMMMMNyyNMMMMMMMMMMMMdo-\n");
  printf("                              -yNMMMMMMMMMMMMms:    :smMMMMMMMMMMMMNy-\n");
  printf("                              yMMMMMMMMMMMh+.          .+hMMMMMMMMMMMy\n");
  printf("                              yMMMMMMMMMMMMh+.        .+hNMMMMMMMMMMMy\n");
  printf("                              `/ymMMMMMMMMMMMMms:  :smMMMMMMMMMMMMMMMy\n");
  printf("                                  -odMMMMMMMMMMMMNNMMMMMMMMMMMMMMMMMMy\n");
  printf("                                     `/yNMMMMMMMMMMMMMMMMMMms+MMMMMMMy\n");
  printf("                                         -odMMMMMMMMMMMMh+.  .MMMMMMMy\n");
  printf("                                            `/yNMMMMms:`     -MMMMMMMy\n");
  printf("                                                -oo-         -MMMMMMMy\n");
  printf("                                     `.                      :MMMMMMMy\n");
  printf("                                  -odMMms:`              `:smMMMMMMMMy\n");
  printf("                              `/yNMMMMMMMMMh+.        .+hMMMMMMMMMMMMy\n");
  printf("                              `:smMMMMMMMMMMMMms:  :smMMMMMMMMMMMMms:`\n");
  printf("                                  .+hMMMMMMMMMMMMNNMMMMMMMMMMMMdo-\n");
  printf("                                     `:smMMMMMMMMMMMMMMMMMMNy/`\n");
  printf("                                         .+hMMMMMMMMMMMMdo-\n");
  printf("                                            `/ymMMMMNy/`\n");
  printf("                                                -oo-\n");

  auth_success = auth(user);
  if (auth_success == 1 ) {
    arret = FAUX;
  }
  else{
    arret = VRAI;
  }

  while (arret == FAUX) {
    printf("%s> ",user);
    if (fgets(texte, LIGNE_MAX, stdin) == NULL) {
      printf("Fin de fichier (ou erreur) : arret.\n");
      arret = VRAI;
      continue;
    }
    else {
      nbecr = ecrireLigne(sock, texte);
      if (nbecr == -1) {
        erreur_IO("ecrireLigne");
      }
      if (strcmp(texte, "fin\n") == 0) {
        printf("Client. arret demande.\n");
        arret = VRAI;
      }
      else {
        printf("%s: ligne de %d octets envoyee au serveur.\n", CMD, nbecr);
      }
    }
  }

  if (close(sock) == -1) {
    erreur_IO("close socket");
  }

  exit(EXIT_SUCCESS);
}
