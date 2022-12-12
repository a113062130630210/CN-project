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
#include <sstream>
// modify from System Programming's hw1 code 
#define PORT 9413
#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)
#define WEBPAGE_LEN 1000000
using namespace std;

char webpage[WEBPAGE_LEN];
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

void register_write_to_file(){
    char *username_tmp;
    char *password_tmp;
    char username[1000];
    char password[1000];
    username_tmp = strstr(p.buf, "username");
    username_tmp = username_tmp + 9;
    int index = 0;
    while(username_tmp[index] != '&'){
        username[index] = username_tmp[index];
        index++;
    }
    password_tmp = strstr(p.buf, "password");
    password_tmp = password_tmp + 9;
    index = 0;
    while(password_tmp[index] != '&'){
        password[index] = password_tmp[index];
        index++;
    }
    FILE *fp = fopen("UserInfo.txt", "a");
    fprintf(fp, "%s %s\n", username, password);

    fclose(fp);
}

bool check_if_account_exists(){
    char *username_tmp;
    char *password_tmp;
    char username[1000];
    char password[1000];
    char filecontent[100000];
    username_tmp = strstr(p.buf, "username");
    username_tmp = username_tmp + 9;
    int index = 0;
    while(username_tmp[index] != '&'){
        username[index] = username_tmp[index];
        index++;
    }
    password_tmp = strstr(p.buf, "password");
    password_tmp = password_tmp + 9;
    index = 0;
    while(password_tmp[index] != '&'){
        password[index] = password_tmp[index];
        index++;
    }
    FILE *fp = fopen("UserInfo.txt", "r");
    fseek(fp, 0, SEEK_SET);
    fread(filecontent, 100000, 1, fp);
    fclose(fp);
    char *find_username = strstr(filecontent, username);
    if(find_username == NULL){
        // didn't find the username in the database
        return false;
    }
    char db_username[1000];
    char db_password[1000];
    sscanf(find_username, "%s %s\n", db_username, db_password);
    if(strcmp(username, db_username) == 0 && strcmp(password, db_password) == 0) return true;
    return false;
}

void HTTP_send_file(int connect_fd, string filename){
    FILE *fp = fopen(filename.c_str(), "r");
    string http_header = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Server: cloudflare\r\n"
    "Connection: Keep-Alive\r\n";
    send(connect_fd, http_header.c_str(), (size_t)http_header.size(), 0);
    memset(webpage, '\0', WEBPAGE_LEN);
    fseek(fp, 0, SEEK_SET);
    fread(webpage, WEBPAGE_LEN, 1, fp);
    fclose(fp);
    send(connect_fd, webpage, strlen(webpage), 0);
}

void handle_http_request(){
    printf("client message = \n%s\n", p.buf);  
    char request_cut_third[5];
    char method[100], path[100], http[100];
    sscanf(p.buf, "%s %s %s", method, path, http);
    strncpy(request_cut_third, http, 4);
    // it is HTTP request
    if(strcmp(request_cut_third, "HTTP") == 0){
        if(strcmp(path, "/") == 0 && strcmp(method, "GET") == 0) HTTP_send_file(connect_fd, "index.php");
        else if(strcmp(path, "/register.html") == 0 && strcmp(method, "GET") == 0) HTTP_send_file(connect_fd, "register.html");
        else if(strcmp(path, "/login.php") == 0 && strcmp(method, "GET") == 0) HTTP_send_file(connect_fd, "login.php");
        //else if(strcmp(path, "/register.php") == 0) HTTP_send_file(connect_fd, "register.php"); 
        else if(strcmp(path, "/index.php") == 0 && strcmp(method, "GET") == 0) HTTP_send_file(connect_fd, "index.php");
        else if(strcmp(path, "/register.php") == 0 && strcmp(method, "POST") == 0){
            HTTP_send_file(connect_fd, "register.php");
            register_write_to_file();
        }
        else if(strcmp(path, "/login.php") == 0 && strcmp(method, "POST") == 0){
            bool does_account_exist = check_if_account_exists();
            cout << does_account_exist << "\n";
            exit(0);        
        }
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
        memset(p.buf, '\0', 4096);
        //read(connect_fd, &p, sizeof(p));
        if(read(connect_fd, &p, sizeof(p)) == 0){
            cout << "client disconnected\n";
            close(connect_fd);
            continue;
        }
        handle_http_request();
        close(connect_fd);
    }
}
