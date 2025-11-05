#include "serveur.h"

int main() {
    char adresseServeur[64] = {0};
    char portServeur[16] = {0};
    char rootServeur[64] = {0};

    chargerConfig("etc/server.conf", adresseServeur, portServeur, rootServeur);
    startServeur(adresseServeur, portServeur, rootServeur);
}