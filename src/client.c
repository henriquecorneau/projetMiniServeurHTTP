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
int chargerConfig(const char *nomFichier, char *ip, char *port);

int main(int argc, char **argv) {
    char adresseServeur[64] = {0};
    char portServeur[16] = {0};
    char buffer[TAILLE_BUFFER] = {0};

    chargerConfig("etc/server.conf", adresseServeur, portServeur);

    if (argc >= 2) {
        strncpy(adresseServeur, argv[1], 63);
        adresseServeur[63] = '\0';
    }

    if (argc >= 3) {
        strncpy(portServeur, argv[2], 15);
        portServeur[15] = '\0';
    }

    printf("Contacting server %s:%s\n", adresseServeur, portServeur);
    
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