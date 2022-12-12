// #include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
// modify from System Programming's hw1 code 
#define PORT 9413
#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)
using namespace std;
typedef struct{
    char buf[4096];
    int length;
}packet;


int main(int argc, char** argv){
	int client_fd, connect_fd;
	char message[4096];
    packet p;
    p.length = 0;
    struct sockaddr_in server_address;
    int server_length = sizeof(server_address);
    while(1){
        if ((connect_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            ERR_EXIT("socket");
        }

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);

        // Convert IPv4 and IPv6 addresses from text to binary
        // form

        if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
            ERR_EXIT("invalid_address");
        }
        

        if((client_fd = connect(connect_fd, (struct sockaddr*)&server_address, server_length)) < 0){
            ERR_EXIT("connect");
        }
        printf("Please say something to server: ");
        //scanf("%s", message);
        fgets(message, 100, stdin);
        strcpy(p.buf, message);
        p.length = strlen(message);
        send(connect_fd, &p, sizeof(packet), 0);
        read(connect_fd, &p, sizeof(p));
        cout << p.buf << endl;	
    }
    close(client_fd);
}