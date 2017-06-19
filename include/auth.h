/* Structure des données d'authentification */
typedef struct login{
	char username[20];
	char password[20];
}logins;

/* Variables globales */

logins login_array[2];

/* Fonctions implémentées */
int auth(char user[]);
void remplir_login_array(logins login_array[2]);
