#include "header.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_IP "10.14.252.29"

int server_down;
int sock_fd;

void myhandler(int signum)
{
    // char msg[25] = "EXIT";
    msg m1;
    strcpy(m1.message, "EXIT");
    send(sock_fd, &m1, sizeof(m1), 0);
    exit(1);
}

void *recv_thread(void *arg)
{

    int *sock_fd = (int *)arg;
    // char msg[512];
    msg ms;
    int n;
    while ((n = recv(*sock_fd, &ms, sizeof(ms), 0)) > 0)
    {
        // msg[n] = '\0';
        printf("%s : %s\n", ms.username, ms.message);

        if (!strcmp(ms.message, "Server Crashed"))
        {
            printf("Server crashed. Closing client...\n");
            server_down = 1;
            close(*sock_fd); 
            raise(SIGKILL);
        }
    }

    return NULL;
}

int main()
{

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    pthread_t mythread;

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    signal(SIGINT, myhandler);
    // signal(SIGTSTP,myhandler);

    package user_info;
    memset(&user_info, 0, sizeof(user_info));
    int choice;
    printf("1. Login\n");
    printf("2.Sign Up\n");
    printf("Enter your choice : ");
    scanf("%d", &choice);
    getchar();

    printf("Enter Username : ");
    scanf("%[^\n]", user_info.username);
    getchar();
    printf("Enter Password : ");
    scanf("%[^\n]", user_info.password);
    getchar();

    if (choice == 1)
    {
        user_info.option = LOGIN;
    }
    else
    {
        user_info.option = SIGN_UP;
    }

    send(sock_fd, &user_info, sizeof(user_info), 0);

    Ack_packet recv_pack;
    recv(sock_fd, &recv_pack, sizeof(recv_pack), 0);

    if (recv_pack.option == ERROR)
    {
        printf("Error : %s\n", recv_pack.message);
        return 0;
    }
    else
    {
        printf("Success : %s\n", recv_pack.message);
    }

    pthread_create(&mythread, NULL, recv_thread, &sock_fd);

    pthread_detach(mythread);

    int choice2;

    do
    {
        if(server_down){
            break;
        }
        printf("In Chat Operation\n");
        printf("1.Single Chat\n");
        printf("2.Group Chat\n");
        printf("3.Logout\n");
        scanf("%d", &choice2);

        switch (choice2)
        {
        case 1:
        {
            char username[25];
            char message[256];
            printf("Enter username want to send message : ");
            getchar();
            scanf("%[^\n]", username);

            printf("Enter message : ");
            getchar();
            scanf("%[^\n]", message);

            // char buff[512];
            // snprintf(buff,sizeof(buff),"%s: %s",username,message);

            msg ms1;
            strcpy(ms1.message, message);
            strcpy(ms1.username, username);
            send(sock_fd, &ms1, sizeof(ms1), 0);
        }
        break;
        case 2:
        {
            char message[256];
            printf("Enter message : ");
            getchar();
            scanf("%[^\n]", message);

            // char buff[512];
            // snprintf(buff,sizeof(buff),"Group: %s",message);
            msg ms2;
            strcpy(ms2.username, "Group");
            strcpy(ms2.message, message);
            send(sock_fd, &ms2, sizeof(ms2), 0);
        }
        break;
        case 3:
        {
            // char msg[25] = "EXIT";
            msg ms3;
            strcpy(ms3.message, "EXIT");
            send(sock_fd, &ms3, sizeof(ms3), 0);

        }
        }

    } while (choice2 != 3);

    return 0;
}