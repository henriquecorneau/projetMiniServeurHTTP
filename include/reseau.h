#ifndef RESEAU_H
#define RESEAU_H

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct sockaddr_in construireAdresseTCPUDPDepuisChaine(char *adresseIP, char *port);
int creerSocketTCPClient(struct sockaddr_in adresse);
int creerSocketTCPServeur(struct sockaddr_in adresse);
int creerSocketTCPUDPServeur(struct sockaddr_in adresse, int mode);

#endif