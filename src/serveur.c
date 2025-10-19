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

