#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define PORT 1234
#define SERVER_IP "132.227.114.36"

#define IS_DEBUG 0
#define DEBUG(s) if (IS_DEBUG) {s;}

const char *HELLO = "0x22222222";
const char *RETURN_HELLO = "0x01";

const uint32_t HELLO_CODE = 0x22222222;
const uint8_t RETURN_CODE = 0x01;

const char *PASSWORD_TEST = "azerty";

int main(int argc, char const *argv[]) 
{    
    int socketFd, clientFd;
    struct sockaddr_in serverAddr;
    int addrLen = sizeof(serverAddr);
    char *msg = "Hello from Server";
    char buffer[1024] = {0};
    char username[30] = {0}, passwordHashed[1024] = {0};

    //Create a socket
    if ((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0 || socketFd == -1) {
        assert(0);
    }
    DEBUG(printf("%d\n", socketFd));
    //Bind socket to Ip address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);   
    //serverAddr.sin_addr.s_addr = INADDR_ANY; //localhost aka 127.0.0.1
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
    
    if ((bind(socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) < 0) {
        assert(0);
    }

    if (listen(socketFd, 2) < 0) {
        assert(0);
    }

    if ((clientFd = accept(socketFd, (struct sockaddr *)&serverAddr, (socklen_t *)&addrLen)) < 0) {
        assert(0);
    }

    printf("init server...\n");

    //Receive a message from client
    bzero(buffer, 1024);
    read(clientFd, buffer, 32);        
    if (*((uint32_t*)buffer) == HELLO_CODE) {
        printf("Receive [%s]\n", HELLO);
        //Send a message
        printf("Sending return [%s]\n", RETURN_HELLO);
        send(clientFd, (char*)&RETURN_CODE, strlen((char*)&RETURN_CODE), 0);

        //Receive a message from client
        read(clientFd, username, 30);
        printf("Received username [%s]\n", username);
        //Send challenge
        char CHALLENGE[16] = {0};
        for(int i = 0;i < 16;i++) {
            CHALLENGE[i] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[rand () % 26];
        }
        printf("Challenge generated [%s]\n", CHALLENGE);
        send(clientFd, CHALLENGE, strlen(CHALLENGE), 0);

        
        char nbuffer[4096] = {0};
        //printf("---->[%s]", nbuffer);
        strncat(nbuffer, CHALLENGE, strlen(CHALLENGE));
        //printf("-->[%s]", nbuffer);
        strncat(nbuffer, PASSWORD_TEST, strlen(PASSWORD_TEST));
        
        //Receive a message from client
        read(clientFd, passwordHashed, 1024);
        //printf("[%s] %d == [%s] %d\n", nbuffer, strlen(nbuffer), passwordHashed, strlen(passwordHashed));
        printf("Received password Hashed with challenge [%s]\n", passwordHashed);
        
        if (strncmp(passwordHashed, nbuffer, strlen(passwordHashed)) == 0) {
            send(clientFd, "OK", 2, 0);
        } else {
            send(clientFd, "NOK", 3, 0);
        }
    } else {
        printf("[%s] != [%s] %d\n", buffer, HELLO, strlen(HELLO));
        send(clientFd, "NOK", 3, 0);
    }
    
    close(clientFd);
    shutdown(socketFd, SHUT_RDWR);
}
