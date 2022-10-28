#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <arpa/inet.h>
#include <string.h>
#include <openssl/sha.h>

#define PORT 12345
#define SERVER_IP "132.227.114.36"

#define IS_DEBUG 0
#define DEBUG(s) if (IS_DEBUG) {s;}

SHA256_CTX ctx;
unsigned char SHA_buffer[32];


const char *HELLO = "0x22222222";
const char *RETURN_HELLO = "0x01";

const uint32_t HELLO_CODE = 0x22222222;
const uint8_t RETURN_CODE = 0x01;

//char CHALLENGE = "";
const char *username = "user123";
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
    printf("Sending to Server [%s]\n", HELLO);
    //send(socketFd, HELLO, strlen(HELLO), 0);
    send(socketFd, (char*)&HELLO_CODE, strlen((char*)&HELLO_CODE), 0);

    //Receive a message from server
    read(socketFd, buffer, 1024);

    if (*((uint8_t*)buffer) == RETURN_CODE) {
        printf("Received [%s]\n", RETURN_HELLO);
        //Send a message
        printf("Sending Username [%s]\n", username);
        send(socketFd, username, 7, 0);
        bzero(buffer, 1024);
        //Receive token from server
        read(socketFd, buffer, 1024);
        printf("Received Challenge [%s]\n", buffer);

        //Hashing sha256
        SHA256_Init(&ctx);
        SHA256_Update(&ctx, PASSWORD_TEST, 32);
        SHA256_Final(SHA_buffer, &ctx);

        char nbuffer[4096] = {0};
        strcat(nbuffer, buffer);
        strcat(nbuffer, SHA_buffer);

        //Send password
        printf("Sending Password with Salt [%s]\n", nbuffer);
       

        send(socketFd, SHA_buffer, strlen(SHA_buffer), 0);

        bzero(buffer, 1024);
        //Receive a message from server
        read(socketFd, buffer, 1024);
        printf("Received Auth Permission [%s]\n", buffer);
    }

    close(serverFd);
    close(socketFd);
}
