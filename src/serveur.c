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

int ecouter(int socketServeur);
int traiterRequete(int socketServeur, char *datagram, struct sockaddr_in adresseClient, int longueurAdresseClient);

int main(int argc, char **argv) {
    char *adresseServeur = NULL;
    char *portServeur = "9099";
    int socketServeur = 0;

    if (argc >= 2) {
        adresseServeur = argv[1];
    }
    else {
        adresseServeur = "127.0.0.1";
    }
    if (argc >= 3) {
        portServeur = argv[2];
    }

    socketServeur = creerSocketUDPServeur(construireAdresseTCPUDPDepuisChaine(adresseServeur, portServeur));

    if (socketServeur != -1) {
        ecouter(socketServeur);
    }

    return 1;
}

int ecouter(int socketServeur) {
    socklen_t longueurAdresseClient = 0;
    struct sockaddr_in adresseClient;
    int erreur = 0;
    char datagram[TAILLE_BUFFER] = {0};
    int nbOctets = 0;

    printf("Server is listening...\n");
    while (erreur != -1) {
        longueurAdresseClient = sizeof(adresseClient);
        nbOctets = recvfrom(socketServeur, datagram, sizeof(datagram), 0, (struct sockaddr *)&adresseClient, &longueurAdresseClient);

        if (nbOctets != -1) {
            printf("Received request\n");
            datagram[nbOctets] = 0;
            nbOctets = traiterRequete(socketServeur, datagram, adresseClient, longueurAdresseClient);
        }
        else {
            erreur = -1;
        }
    }
    close(socketServeur);
    return erreur;
}

int traiterRequete(int socketServeur, char *datagram, struct sockaddr_in adresseClient, int longueurAdresseClient) {
    time_t heureCourante;
    int nbCaracteres = 0;
    char buffer[TAILLE_BUFFER] = {0};

    time(&heureCourante);
    nbCaracteres = (int) strftime(buffer, TAILLE_BUFFER, datagram, localtime(&heureCourante));

    return sendto(socketServeur, buffer, nbCaracteres, 0, (struct sockaddr *)&adresseClient, longueurAdresseClient);
}

