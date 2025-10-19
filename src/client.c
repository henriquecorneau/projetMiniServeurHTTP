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
int envoyerRequete(int socket);
int lireResultat(int socket, char *buffer);
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
    int socketTCP = 0;

    addrServeur = construireAdresseTCPUDPDepuisChaine(adresseServeur, portServeur);
    socketTCP = creerSocketTCPClient(addrServeur);

    if (socketTCP != -1) {
        envoyerRequete(socketTCP);
        lireResultat(socketTCP, resultat);
        close(socketTCP);
    }
    else {
        resultat[0] = '\0';
    }
}

int envoyerRequete(int socket) {
    char formatDateHeure[]="%A %b %d %H:%M:%S %Y\n";
    return write(socket,formatDateHeure,strlen(formatDateHeure));
}

int lireResultat(int socket, char *buffer) {
    int nbCaracteres = 0;
    nbCaracteres = read(socket, buffer, TAILLE_BUFFER - 1);

    if (nbCaracteres != -1) {
        buffer[nbCaracteres] = '\0';
    }

    return nbCaracteres;
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