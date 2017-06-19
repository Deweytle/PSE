#include "pse.h"

int auth_success = 0;

void remplir_login_array(logins login_array[2]){
//User 1
  strcpy(login_array[0].username, "Deweytle");
  strcpy(login_array[0].password, "Deweytle");
//User 2
  strcpy(login_array[1].username, "User");
  strcpy(login_array[1].password, "User");
}

int auth(char user[]){
  char pswd[20];
  printf("Username: ");
  scanf("%s", user);
  printf("\nPassword: ");
  scanf("%s", pswd);
  printf("\n");
  for (int taille_array = 0; taille_array < 2; taille_array++) {
    if(strcmp(login_array[taille_array].username,user)==0 && strcmp(login_array[taille_array].password,pswd)==0){
      auth_success = 0;
    }
    else{
      auth_success = 1;
    }
  }
  return auth_success;
}
