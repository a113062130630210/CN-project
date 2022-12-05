#include <bits/stdc++.h>
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
// modify from System Programming's hw1 code 
#define PORT 9413
#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)
using namespace std;

typedef struct {
    string hostname;  // server's hostname
    int listen_fd;  // fd to wait for a new connection
} server;

typedef struct{
    char buf[4096];
    int length;
} packet;

char message[4096];
int connect_fd;

typedef struct {
    char host[512];  // client's host
    int conn_fd;  // fd to talk with client
    char buf[512];  // data sent by/to client
    size_t buf_len;  // bytes used by buf
} request;

server svr;
packet p;

request* requestP = NULL;
int max_connection;
struct pollfd fd_array[50];
int len_fd_array;
struct sockaddr_in server_address;

void handle_http_request(){
    char request_cut[5];
    char webpage[10000];
    char get[100], path[100], http[100];
    sscanf(p.buf, "%s %s %s", get, path, http);
    strncpy(request_cut, http, 4);
    // it is HTTP request
    if(strcmp(request_cut, "HTTP") == 0){
        //cout << buf << endl;
        string http_header = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8"
        "Server: cloudflare";
        send(connect_fd, http_header.c_str(), (size_t)http_header.size(),0);
        FILE *fp = fopen("page.html", "r");
        fread(webpage, 10000, 1, fp);
        fclose(fp);
        send(connect_fd, webpage, strlen(webpage), 0);
    }else{
        //cout << "HTTP doesn't received\n";
        cout << p.buf << endl;
        printf("Please say something to client: ");
        fgets(message, 100, stdin); 
        strcpy(p.buf, message);
        p.length = strlen(message);
        send(connect_fd, &p, sizeof(p), 0);
    }
}

void start_server(){
    int tmp = 1;
    max_connection = 100;
    if ((svr.listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        ERR_EXIT("socket");
    }

    if (setsockopt(svr.listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (void*)&tmp, sizeof(tmp)) < 0) {
        ERR_EXIT("setsockopt");
    }
    // Forcefully attaching socket to the port
    
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);

    if(bind(svr.listen_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        ERR_EXIT("bind");
    }

    if (listen(svr.listen_fd, 1024) < 0) {
        ERR_EXIT("listen");
    }
    return;
}

void init_request(request* req) {
    req->conn_fd = -1;
    req->buf_len = 0;
}

void free_request(request* req) {
    init_request(req);
}

int main(int argc, char** argv){
    start_server();
    int server_length = sizeof(server_address);
    // read from client
    while(1){
        if((connect_fd = accept(svr.listen_fd, (struct sockaddr*)&server_address, (socklen_t*)&(server_length))) < 0){
            ERR_EXIT("accept");
        }
        if(read(connect_fd, &p, sizeof(p)) == 0){
            cout << "client disconnected\n";
            close(connect_fd);
            continue;
        }
        handle_http_request();
        //send(connect_fd, message, strlen(message), 0);
        close(connect_fd);
    }
}
