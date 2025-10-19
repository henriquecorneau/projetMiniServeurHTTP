#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "reseau.h"

#define TAILLE_BUFFER 128

int ecouter(int socketServeur, int longueurFileDAttente);
int traiterRequete(int socketClient);
int lireRequete(int socket, char *formatDateHeure);
int envoyerResultat(int socket, char *formatDateHeure);
int chargerConfig(const char *nomFichier, char *ip, char *port);

int main(int argc, char **argv) {
    char adresseServeur[64] = {0};
    char portServeur[16] = {0};
    int socketServeur = 0;

    chargerConfig("etc/server.conf", adresseServeur, portServeur);

    if (argc >= 2) {
        strncpy(adresseServeur, argv[1], 63);
        adresseServeur[63] = '\0';
    }

    if (argc >= 3) {
        strncpy(portServeur, argv[2], 15);
        portServeur[15] = '\0';
    }

    printf("Starting server on %s:%s\n", adresseServeur, portServeur);
    socketServeur = creerSocketTCPServeur(construireAdresseTCPUDPDepuisChaine(adresseServeur, portServeur));

    if (socketServeur != -1) {
        ecouter(socketServeur, 10);
    }

    return 1;
}

int ecouter(int socketServeur, int longueurFileDAttente) {
    socklen_t longueurAdresseClient = 0;
    struct sockaddr_in adresseClient;
    int erreur = 0;
    int socketClient = 0;

    printf("Server is listening...\n");
    erreur = listen(socketServeur, longueurFileDAttente);
    while (erreur != -1) {
        longueurAdresseClient = sizeof(adresseClient);
        socketClient = accept(socketServeur, (struct sockaddr *)&adresseClient, &longueurAdresseClient);

        if (socketClient != -1) {
            if (traiterRequete(socketClient)) {
                printf("Request processed successfully.\n");
                close(socketClient);
            }
            else {
                erreur = -1;
            }
        }
        else {
            erreur = -1;
        }
    }  
    return erreur;
}

int traiterRequete(int socketClient) {
    char formatDateHeure[TAILLE_BUFFER] = {0};

    if (lireRequete(socketClient, formatDateHeure) != -1) {
        return envoyerResultat(socketClient, formatDateHeure);
    }
    else {
        return -1;
    }
}

int lireRequete(int socket, char *formatDateHeure) {
    int nbCaracteres = 0;
    nbCaracteres = read(socket, formatDateHeure, TAILLE_BUFFER - 1);

    if (nbCaracteres != -1) {
        formatDateHeure[nbCaracteres] = '\0';
    }
    return nbCaracteres;
}

int envoyerResultat(int socket, char *formatDateHeure) {
    time_t heureCourante;
    char buffer[TAILLE_BUFFER] = {0};
    int nbCaracteres = 0;
    time(&heureCourante);
    nbCaracteres = (int)strftime(buffer, sizeof(buffer), formatDateHeure, localtime(&heureCourante));

    return write(socket, buffer, nbCaracteres);
}

int chargerConfig(const char *nomFichier, char *ip, char *port) {
    FILE *file = fopen(nomFichier, "r");
    if (!file) {
        perror("Error opening config file");
        return -1;
    }

    char line[128];
    ip[0] = '\0';
    port[0] = '\0';

    while (fgets(line, sizeof(line), file)) {
        // remove \n
        line[strcspn(line, "\n")] = 0;

        // ignore comments and empty lines
        if (line[0] == '\0' || line[0] == '#')
            continue;

        if (strncmp(line, "ip=", 3) == 0) {
            strncpy(ip, line + 3, 63);
            ip[63] = '\0';
        } else if (strncmp(line, "port=", 5) == 0) {
            strncpy(port, line + 5, 15);
            port[15] = '\0';
        }
    }

    fclose(file);

    if (ip[0] == '\0' || port[0] == '\0') {
        fprintf(stderr, "Error ip or port empty\n");
        return -1;
    }

    return 0; 
}

