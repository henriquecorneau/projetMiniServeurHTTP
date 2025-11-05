#include "reseau.h"

struct sockaddr_in construireAdresseTCPUDPDepuisChaine(char *adresseIP, char *port) {
    struct sockaddr_in adresse;

    memset(&adresse, 0, sizeof(adresse));
    adresse.sin_family = AF_INET;
    adresse.sin_port = htons(atoi(port));
    adresse.sin_addr.s_addr = inet_addr(adresseIP);

    return adresse;
}

int creerSocketTCPUDPServeur(struct sockaddr_in adresse, int mode) {
    int resultat = 0;
    int longueurAdresse = 0;

    resultat = socket(PF_INET, mode, 0);
    if ((resultat == -1) || (adresse.sin_addr.s_addr == INADDR_NONE)) {
        return -1;
    }
    else {
        longueurAdresse = sizeof(adresse);
        if (bind(resultat, (struct sockaddr *)&adresse, longueurAdresse) != -1) {
            return resultat;
        }
        else {
            return -1;
        }
    }
}

int creerSocketTCPClient(struct sockaddr_in adresse) {
    int resultat = 0;
    int longueurAdresse = 0;

    resultat = socket(PF_INET, SOCK_STREAM, 0);
    if ((resultat == -1) || (adresse.sin_addr.s_addr == INADDR_NONE)) {
        return -1;
    }
    else {
        longueurAdresse = sizeof(adresse);
        if (connect(resultat, (struct sockaddr *)&adresse, longueurAdresse) != -1) {
            return resultat;
        }
        else {
            return -1;
        }
    }
}

int creerSocketTCPServeur(struct sockaddr_in adresse) {
    return creerSocketTCPUDPServeur(adresse, SOCK_STREAM);
}