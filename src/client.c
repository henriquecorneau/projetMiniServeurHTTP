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

void recupererDateEtHeure(char *adresseServeur, char *portServeur, char *resultat);
int envoyerRequete(int socket, struct sockaddr_in adresseServeur);
int lireResultat(int socket, char *datagram);

int main(int argc, char **argv) {
    char *adresseServeur = NULL;
    char *portServeur = "9099";
    char buffer[TAILLE_BUFFER] = {0};

    if (argc >= 2) {
        adresseServeur = argv[1];
    }
    else {
        adresseServeur = "127.0.0.1";
    }

    if (argc >= 3) {
        portServeur = argv[2];
    }

    recupererDateEtHeure(adresseServeur, portServeur, buffer);
    printf("%s", buffer);
    return 1;
}

void recupererDateEtHeure(char *adresseServeur, char *portServeur, char *resultat) {
    struct sockaddr_in addrServeur;
    int socketUDP = 0;
    int nbCaracteres = 0;

    addrServeur = construireAdresseTCPUDPDepuisChaine(adresseServeur, portServeur);
    socketUDP = socket(PF_INET, SOCK_DGRAM, 0);

    if (socketUDP != -1) {
        envoyerRequete(socketUDP, addrServeur);
        nbCaracteres = lireResultat(socketUDP, resultat);

        if (nbCaracteres != -1) {
            resultat[nbCaracteres] = '\0';
        }
        close(socketUDP);
    }
    else {
        resultat[0] = '\0';
    }
}

int envoyerRequete(int socket, struct sockaddr_in adresseServeur) {
    char datagram[]="%A %b %d %H:%M:%S %Y\n";
    int longuerAdresseServeur = sizeof(adresseServeur);
    return sendto(socket, datagram, sizeof(datagram), 0, (struct sockaddr *)&adresseServeur, longuerAdresseServeur);
}

int lireResultat(int socket, char *datagram) {
    struct sockaddr_in adresseReponse;
    socklen_t longueurAdresseReponse = sizeof(adresseReponse);
    return recvfrom(socket, datagram, TAILLE_BUFFER, 0, (struct sockaddr *)&adresseReponse, &longueurAdresseReponse);
}

