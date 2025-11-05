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
#include "serveur.h"

char dirServeur[64] = {0};

void startServeur(char *ipServeur, char *portServeur, char *rootServeur) {
    int socketServeur = 0;
    strcpy(dirServeur, rootServeur);

    printf("Starting server on %s:%s\n", ipServeur, portServeur);
    printf("Server directory: %s\n", dirServeur);
    socketServeur = creerSocketTCPServeur(construireAdresseTCPUDPDepuisChaine(ipServeur, portServeur));

    if (socketServeur != -1) {
        ecouter(socketServeur, 10);
    }
}

int ecouter(int socketServeur, int longueurFileDAttente) {
    socklen_t longueurAdresseClient = 0;
    struct sockaddr_in adresseClient;
    int erreur = 0;
    int socketClient = 0;
    pid_t PID;

    printf("Server is listening...\n");
    erreur = listen(socketServeur, longueurFileDAttente);
    while (erreur != -1) {
        longueurAdresseClient = sizeof(adresseClient);
        socketClient = accept(socketServeur, (struct sockaddr *)&adresseClient, &longueurAdresseClient);

        if (socketClient != -1) {
            if ((PID = fork()) != -1) {
                if (PID == 0) {
                    if (traiterRequete(socketClient)) {
                        printf("Request processed successfully.\n");
                        close(socketClient);
                    }
                    else {
                        erreur = -1;
                    }
                }
                else {
                    close(socketClient);
                }
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
    char request[TAILLE_BUFFER] = {0};

    if (lireRequete(socketClient, request) != -1) {
        printf("Request :\n%s\n", request);
        return envoyerResultat(socketClient, request);
    }
    else {
        return -1;
    }
}

int lireRequete(int socket, char *request) {
    int nbCaracteres = 0;
    nbCaracteres = read(socket, request, TAILLE_BUFFER - 1);

    if (nbCaracteres != -1) {
        request[nbCaracteres] = '\0';
    }
    return nbCaracteres;
}

int envoyerResultat(int socket, char *request) {
    char buffer[TAILLE_BUFFER] = {0};
    int nbCaracteres = 0;
    char path[64] = {0};
    char fullPath[128] = {0};

    sscanf(request, "GET %255s HTTP/1.0", path);
    if (strcmp(path, "/") == 0) {
        strcpy(path, "/index.html");
    }
    snprintf(fullPath, sizeof(fullPath), "%s%s", dirServeur, path);

    FILE *fichier = fopen(fullPath, "rb");

    if (fichier == NULL) {
        sprintf(buffer, "HTTP/1.0 404 Not Found\r\n\r\n");
        nbCaracteres = strlen(buffer) * sizeof(char);
        return write(socket, buffer, nbCaracteres);
    }

    fseek(fichier, 0, SEEK_END);
    long contentLength = ftell(fichier);
    rewind(fichier);

    const char *contentType = getContentType(fullPath);
    snprintf(buffer, sizeof(buffer),
             "HTTP/1.0 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %ld\r\n"
             "\r\n",
             contentType, contentLength);

    write(socket, buffer, strlen(buffer)* sizeof(char));

    while ((nbCaracteres = fread(buffer, 1, sizeof(buffer), fichier)) > 0) {
        write(socket, buffer, nbCaracteres);
    }

    fclose(fichier);
    return 0;
}

void chargerConfig(const char *nomFichier, char *ip, char *port, char *root) {
    FILE *fp = fopen(nomFichier,"r");

    fscanf(fp, "ip=%s\n port=%s\n root=%s\n", ip, port, root);
    fclose(fp);
}

const char *getContentType(const char *dirFile) {
    const char *extension = strrchr(dirFile, '.');

    if (strcmp(extension, ".html") == 0) {
        return "text/html";
    } else if (strcmp(extension, ".png") == 0) {
        return "image/png";
    } else if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) {
        return "image/jpeg";
    } else {
        return "text/plain";
    }
}

