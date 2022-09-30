#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 1234
#define SERVER_IP "132.227.114.35"

#define IS_DEBUG 0
#define DEBUG(s) if (IS_DEBUG) {s;}

const char *HELLO = "0x22222222";
const char *RETURN_HELLO = "0x01";
//char CHALLENGE = "";

const char *PASSWORD_TEST = "azerty";

int main(int argc, char const *argv[]) 
{    
    int socketFd, serverFd;
    struct sockaddr_in serverAddr;
    char *msg = "Hello from Client";
    char buffer[1024] = { 0 };

    //Create a socket
    if ((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0 || socketFd == -1) {
        assert(0);
    }
    DEBUG(printf("%d\n", socketFd));
    //Connect to server's Ip address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);   
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
    
    if ((serverFd = connect(socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) < 0) {
        assert(0);
    }

    printf("init client...\n");

    //Send a message
    send(socketFd, HELLO, strlen(HELLO), 0);

    //Receive a message from server
    read(socketFd, buffer, 1024);

    if (strncmp(buffer, RETURN_HELLO, strlen(RETURN_HELLO)) == 0) {
        //Send a message
        send(socketFd, "user123", 6, 0);
        bzero(buffer, 1024);
        //Receive token from server
        read(socketFd, buffer, 1024);
        printf("Received Challenge [%s]\n", buffer);

        char nbuffer[4096] = {0};
        strcat(nbuffer, buffer);
        strcat(nbuffer, PASSWORD_TEST);

        //Send password
        send(socketFd, nbuffer, strlen(nbuffer), 0);

        bzero(buffer, 1024);
        //Receive a message from server
        read(socketFd, buffer, 1024);
        printf("Received Auth Permission [%s]\n", buffer);
    }

    close(serverFd);
    close(socketFd);
}