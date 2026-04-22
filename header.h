#ifndef HEADER_H
#define HEADER_H

#define PORT 8085

typedef enum option{
    LOGIN,SIGN_UP,ERROR,SUCCESS
}options;

typedef struct client_{
    int client_fd;
    char username[25];
    struct client_ *next;
}client_node;

typedef struct {
    int client_fd;
}client_t;

typedef struct acknowledge{
    int option;
    char message[50];
}Ack_packet;

typedef struct package{
    char username[25];
    char password[25];
    int option;
}package;

typedef struct messge
{
    char username[25];
    char message[512];
}msg;


#endif