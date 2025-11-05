#ifndef SERVEUR_H
#define SERVEUR_H

#define TAILLE_BUFFER 128

extern char dirServeur[64];

void startServeur(char *ipServeur, char *portServeur, char *rootServeur);
int ecouter(int socketServeur, int longueurFileDAttente);
int traiterRequete(int socketClient);
int lireRequete(int socket, char *formatDateHeure);
int envoyerResultat(int socket, char *formatDateHeure);
void chargerConfig(const char *nomFichier, char *ip, char *port, char *root);
const char *getContentType(const char *dirFile);

#endif