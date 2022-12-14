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
#include <sstream>
//FOR MAC users
#include <cstring>
#include <iostream>
// modify from System Programming's hw1 code 
#define PORT 7777
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

string get_username(){
    char *username_tmp;
    string username = "";
    username_tmp = strstr(p.buf, "username=");
    if(username_tmp == NULL){
        return "";
    }
    username_tmp = username_tmp + 9;
    int index = 0;
    while(username_tmp[index] != '&'){
        username += username_tmp[index];
        index++;
    }

    return username;
}

string get_username_from_cookie(){
    char *cookie_tmp;
    string cookie = "";
    cookie_tmp = strstr(p.buf, "id=");
    if(cookie_tmp == NULL){
        return "";
    }
    cookie_tmp = cookie_tmp + 11;
    int index = 0;
    while(cookie_tmp[index] != '&'){
        cookie += cookie_tmp[index];
        index++;
    }
    
    return cookie;
}

string get_content_from_cookie(){
    //TODO
    char *content_tmp;
    string content = "";
    content_tmp = strstr(p.buf, "message=");
    printf("content_tmp = %s\n", content_tmp);
    if(content_tmp == NULL){
        return "";
    }
    int index = 0;
    content_tmp = content_tmp + 8;
    while(content_tmp[index] != '&'){
        char special_char[100] = "";
        if(content_tmp[index] == '+'){
            content += " ";
        }
        else if(content_tmp[index] == '%'){
            cout << "content_tmp[index] = " << content_tmp[index] << " content_tmp[index + 1] = " << content_tmp[index + 1] << " content_tmp[index + 2] = " << content_tmp[index + 2] << endl;
            strncat(special_char, content_tmp + index, 3);
            cout << "special_char = " << special_char << endl;
            index += 2;
            if(strcmp(special_char, "%20") == 0) content += " ";
            if(strcmp(special_char, "%21") == 0) content += "!";
            //if(special_char == "%22") content += '"';
            if(strcmp(special_char, "%23") == 0) content += "#";
            if(strcmp(special_char, "%24") == 0) content += "$";
            if(strcmp(special_char, "%25") == 0) content += "%";
            if(strcmp(special_char, "%26") == 0) content += "&";
            if(strcmp(special_char, "%27") == 0) content += "'";
            if(strcmp(special_char, "%28") == 0) content += "(";
            if(strcmp(special_char, "%29") == 0) content += ")";
            if(strcmp(special_char, "%2A") == 0) content += "*";
            if(strcmp(special_char, "%2B") == 0) content += "+";
            if(strcmp(special_char, "%2C") == 0) content += ",";
            if(strcmp(special_char, "%2D") == 0) content += "-";
            if(strcmp(special_char, "%2E") == 0) content += ".";
            if(strcmp(special_char, "%2F") == 0) content += "/";
            if(strcmp(special_char, "%3A") == 0) content += ":";
            if(strcmp(special_char, "%3B") == 0) content += ";";
            if(strcmp(special_char, "%3C") == 0) content += "<";
            if(strcmp(special_char, "%3D") == 0) content += "=";
            if(strcmp(special_char, "%3E") == 0) content += ">";
            if(strcmp(special_char, "%3F") == 0) content += "?";
            if(strcmp(special_char, "%7B") == 0) content += "{";
            if(strcmp(special_char, "%7C") == 0) content += "|";
            if(strcmp(special_char, "%7D") == 0) content += "}";
            if(strcmp(special_char, "%7E") == 0) content += "~";
            if(strcmp(special_char, "%84") == 0) content += "„";
            if(strcmp(special_char, "%85") == 0) content += "…";
            if(strcmp(special_char, "%96") == 0) content += "–";
        }
        else{
            content += content_tmp[index];
        }
        index++;
    }
    
    return content;
}

void register_write_to_file(){
    char *username_tmp;
    char *password_tmp;
    string username = "";
    string password = "";
    username_tmp = strstr(p.buf, "username=");
    if(username_tmp == NULL) return;
    username_tmp = username_tmp + 9;
    int index = 0;
    while(username_tmp[index] != '&'){
        username += username_tmp[index];
        index++;
    }
    password_tmp = strstr(p.buf, "password=");
    if(password_tmp == NULL) return;
    password_tmp = password_tmp + 9;
    index = 0;
    while(password_tmp[index] != '&'){
        password += password_tmp[index];
        index++;
    }
    FILE *fp = fopen("database/UserInfo.txt", "a");
    fprintf(fp, "%s %s\n", username.c_str(), password.c_str());
    fclose(fp);
}

bool same_username_account(){
    char *username_tmp;
    string username = "";
    char filecontent[100000];
    memset(filecontent, '\0', 100000);
    username_tmp = strstr(p.buf, "username");
    username_tmp = username_tmp + 9;
    int index = 0;
    while(username_tmp[index] != '&'){
        username += username_tmp[index];
        index++;
    }
    cout << "User input username = " << username << endl;
    FILE *fp = fopen("database/UserInfo.txt", "r");
    fseek(fp, 0, SEEK_SET);
    fread(filecontent, 100000, 1, fp);
    fclose(fp);
    printf("filecontent = %s\n", filecontent);
    char *database_username_tmp = strstr(filecontent, username.c_str());
    string database_username = "";
    printf("find_username = %s\n", database_username_tmp);
    if(database_username_tmp == NULL){
        // there is no user with the same username
        return false;
    }
    index = 0;
    while(database_username_tmp[index] != ' '){
        database_username += username_tmp[index];
        index++;
    }
    if(username != database_username){
        return false;
    }
    return true;
}

bool check_if_is_login(){
    char *cookie_tmp;
    cookie_tmp = strstr(p.buf, "Cookie: id=");
    if(cookie_tmp != NULL){
        if(strlen(cookie_tmp) < 4) return true;
        if(*cookie_tmp == 'N' && *(cookie_tmp + 1) == 'o' && *(cookie_tmp + 2) == 'n' && *(cookie_tmp + 3) == 'e'){
            return false;
        }
        return true;
    }
    return false;
}

bool check_if_account_exists(){
    char *username_tmp;
    char *password_tmp;
    string username = "";
    string password = "";
    char filecontent[100000];
    memset(filecontent, '\0', 100000);
    username_tmp = strstr(p.buf, "username");
    username_tmp = username_tmp + 9;
    int index = 0;
    while(username_tmp[index] != '&'){
        username += username_tmp[index];
        index++;
    }
    password_tmp = strstr(p.buf, "password");
    password_tmp = password_tmp + 9;
    index = 0;
    while(password_tmp[index] != '&'){
        password += password_tmp[index];
        index++;
    }
    cout << "User input username = " << username << " password = " << password << endl;
    FILE *fp = fopen("database/UserInfo.txt", "r");
    fseek(fp, 0, SEEK_SET);
    fread(filecontent, 100000, 1, fp);
    fclose(fp);
    printf("filecontent = %s\n", filecontent);
    char *find_username = strstr(filecontent, username.c_str());
    printf("find_username = %s\n", find_username);
    if(find_username == NULL){
        // didn't find the username in the database
        return false;
    }
    char db_username[1000];
    char db_password[1000];
    memset(db_username, '\0', 1000);
    memset(db_password, '\0', 1000);
    sscanf(find_username, "%s %s\n", db_username, db_password);
    if(strcmp(username.c_str(), db_username) == 0 && strcmp(password.c_str(), db_password) == 0) return true;
    return false;
}

void HTTP_send_file_with_cookie(int connect_fd, string filename, string username){
    FILE *fp = fopen(filename.c_str(), "r");
    string http_header = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Server: cloudflare\r\n"
    "Connection: Keep-Alive\r\n";
    string cookie = "Set-Cookie: " + username + "&\r\n\r\n";
    http_header += cookie;
    //send(connect_fd, http_header.c_str(), (size_t)http_header.size(), 0);
    memset(webpage, '\0', WEBPAGE_LEN);
    fseek(fp, 0, SEEK_SET);
    fread(webpage, WEBPAGE_LEN, 1, fp);
    fclose(fp);
    http_header += webpage;
    send(connect_fd, http_header.c_str(), (size_t)http_header.size(), 0);
    //send(connect_fd, webpage, strlen(webpage), 0);
}

void show_message_setcookie(int connect_fd, string username){
    string http_header = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Server: cloudflare\r\n"
    "Connection: Keep-Alive\r\n";
    string cookie = "Set-Cookie: id=" + username + "&\r\n\r\n";
    http_header += cookie;
    //send(connect_fd, http_header.c_str(), (size_t)http_header.size(), 0);
    memset(webpage, '\0', WEBPAGE_LEN);
    FILE *fp = fopen("webpage/welcome_1.html", "r");
    fseek(fp, 0, SEEK_SET);
    fread(webpage, WEBPAGE_LEN, 1, fp);
    fclose(fp);
    http_header += webpage;
    http_header += "Hi, ";
    http_header += username;
    http_header += ". Welcome to the message board!</h1><h2>Here is the content of message board</h2>";
    fp = fopen("database/messageboard.txt", "a+");
    memset(webpage, '\0', WEBPAGE_LEN);
    fseek(fp, 0, SEEK_SET);
    fread(webpage, WEBPAGE_LEN, 1, fp);
    fclose(fp);
    http_header += webpage;
    http_header += "</br>";
    memset(webpage, '\0', WEBPAGE_LEN);
    fp = fopen("webpage/welcome_2.html", "r");
    fseek(fp, 0, SEEK_SET);
    fread(webpage, WEBPAGE_LEN, 1, fp);
    fclose(fp);
    http_header += webpage;
    send(connect_fd, http_header.c_str(), (size_t)http_header.size(), 0);
}


void HTTP_send_file(int connect_fd, string filename){
    FILE *fp = fopen(filename.c_str(), "r");
    string http_header = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Server: cloudflare\r\n"
    "Connection: Keep-Alive\r\n"
    "\r\n";
    //send(connect_fd, http_header.c_str(), (size_t)http_header.size(), 0);
    memset(webpage, '\0', WEBPAGE_LEN);
    fseek(fp, 0, SEEK_SET);
    fread(webpage, WEBPAGE_LEN, 1, fp);
    fclose(fp);
    http_header += webpage;
    //send(connect_fd, webpage, strlen(webpage), 0);
    send(connect_fd, http_header.c_str(), (size_t)http_header.size(), 0);

    cout << "send to client: \n" << http_header << endl;
}

void HTTP_send_file_clear_cookie(int connect_fd, string filename){
    FILE *fp = fopen(filename.c_str(), "r");
    string http_header = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Server: cloudflare\r\n"
    "Connection: Keep-Alive\r\n"
    "Set-Cookie: id=None\r\n"
    "\r\n";
    //"Set-Cookie: token = deleted; path=/; expires= Thu, 01 Jan 1970 00:00:00 GMT\r\n";
    //send(connect_fd, http_header.c_str(), (size_t)http_header.size(), 0);
    memset(webpage, '\0', WEBPAGE_LEN);
    fseek(fp, 0, SEEK_SET);
    fread(webpage, WEBPAGE_LEN, 1, fp);
    fclose(fp);
    http_header += webpage;
    //send(connect_fd, webpage, strlen(webpage), 0);
    send(connect_fd, http_header.c_str(), (size_t)http_header.size(), 0);
}

void record_to_database(string username, string content){
    FILE *fp = fopen("database/messageboard.txt", "a");
    fprintf(fp, "%s: %s<br/>\n", username.c_str(), content.c_str());
    fclose(fp);
}

void handle_http_request(){
    printf("client message = \n%s\n", p.buf);  
    char request_cut_third[5];
    char method[100], path[100], http[100];
    sscanf(p.buf, "%s %s %s", method, path, http);
    cout << method << " " << path << " " << http << endl;
    strncpy(request_cut_third, http, 4);
    // it is HTTP request
    if(strcmp(request_cut_third, "HTTP") == 0){
        if(strcmp(path, "/") == 0 && strcmp(method, "GET") == 0) HTTP_send_file(connect_fd, "webpage/index.php");
        else if(strcmp(path, "/register.html") == 0 && strcmp(method, "GET") == 0) HTTP_send_file(connect_fd, "webpage/register.html");
        else if(strcmp(path, "/register.html") == 0 && strcmp(method, "POST") == 0) HTTP_send_file(connect_fd, "webpage/register.html");
        else if(strcmp(path, "/login.php") == 0 && strcmp(method, "GET") == 0) HTTP_send_file(connect_fd, "webpage/login.php");
        //else if(strcmp(path, "/register.php") == 0) HTTP_send_file(connect_fd, "webpage/register.php"); 
        else if(strcmp(path, "/index.php") == 0 && strcmp(method, "GET") == 0) HTTP_send_file(connect_fd, "webpage/index.php");
        else if(strcmp(path, "/register.php") == 0 && strcmp(method, "POST") == 0){
            if(same_username_account() == true){
                // there is another user with the same account
                HTTP_send_file(connect_fd, "webpage/registerfail.html");
            }
            else{
                HTTP_send_file(connect_fd, "webpage/register.php");
                register_write_to_file();
            }
        }
        else if(strcmp(path, "/login.php") == 0 && strcmp(method, "POST") == 0){
            // 檢查帳號存不存在，並登入
            bool does_account_exist = check_if_account_exists();
            cout << "account exist = " << does_account_exist << endl;
            if(does_account_exist == 1){
                string username = get_username();
                cout << "username = " << username << endl;
                show_message_setcookie(connect_fd, username);
            }else{
                HTTP_send_file(connect_fd, "webpage/retry.html");
            }
        }
        else if(strcmp(path, "/welcome_1.html") == 0 && strcmp(method, "GET") == 0) HTTP_send_file(connect_fd, "webpage/index.php");
        else if(strcmp(path, "/welcome_1.html") == 0 && strcmp(method, "POST") == 0){
            // 已經在留言板裡面了
            string username = get_username_from_cookie();
            string content = get_content_from_cookie();
            cout << "username = " << username << endl;
            cout << "content = " << content << endl;
            record_to_database(username, content);
            show_message_setcookie(connect_fd, username);
        }
        else if(strcmp(path, "/retry.html") == 0 && strcmp(method, "GET") == 0) HTTP_send_file(connect_fd, "webpage/retry.html");
        else if(strcmp(path, "/logout.php") == 0 && strcmp(method, "POST") == 0) HTTP_send_file_clear_cookie(connect_fd, "webpage/index.php");
        else HTTP_send_file(connect_fd, "webpage/404.html");
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

    if (setsockopt(svr.listen_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&tmp, sizeof(tmp)) < 0) {
        ERR_EXIT("setsockopt");
    }
    // Forcefully attaching socket to the port
    
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if(::bind(svr.listen_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
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
        cout << "client connected\n";
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
