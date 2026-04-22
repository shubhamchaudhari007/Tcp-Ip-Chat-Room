#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "header.h"
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define CLIENT_LEN 10

pthread_mutex_t my_lock;
pthread_mutex_t list_lock;

client_node *head = NULL;

void user_online(const char *username, int new_fd)
{
    msg ms;
    memset(&ms, 0, sizeof(ms));

    strcpy(ms.username, "SERVER");
    snprintf(ms.message, sizeof(ms.message),
             "%s is online", username);

    pthread_mutex_lock(&list_lock);

    client_node *curr = head;
    while (curr) {
        if (curr->client_fd != new_fd) {
            send(curr->client_fd, &ms, sizeof(ms), 0);
        }
        curr = curr->next;
    }

    pthread_mutex_unlock(&list_lock);
}

void send_online_users(int new_fd)
{

    pthread_mutex_lock(&list_lock);

    client_node *curr = head;
    msg ms1;
    memset(&ms1, 0, sizeof(ms1));

    while (curr) {
        if (curr->client_fd != new_fd) {
            strcpy(ms1.username,"SERVER");
            snprintf(ms1.message, sizeof(ms1.message),
                     "%s is online\n", curr->username);
            send(new_fd, &ms1, sizeof(ms1), 0);
        }
        curr = curr->next;
    }

    pthread_mutex_unlock(&list_lock);
}

void myhandler(int signum)
{

    msg ms1;
    memset(&ms1, 0, sizeof(ms1));
    strcpy(ms1.username,"SERVER");
    strcpy(ms1.message,"Server Crashed");

    pthread_mutex_lock(&list_lock);
    client_node *curr = head;
    while (curr)
    {
        send(curr->client_fd, &ms1, sizeof(ms1), 0);

        curr = curr->next;
    }
    exit(1);
}

void user_offline(const char *username, int new_fd)
{
    msg ms;
    memset(&ms, 0, sizeof(ms));

    strcpy(ms.username, "SERVER");
    snprintf(ms.message, sizeof(ms.message),
             "%s is offine", username);

    pthread_mutex_lock(&list_lock);

    client_node *curr = head;
    while (curr) {
        if (curr->client_fd != new_fd) {
            send(curr->client_fd, &ms, sizeof(ms), 0);
        }
        curr = curr->next;
    }

    pthread_mutex_unlock(&list_lock);
}

void add_client(int client_fd, char *username)
{

    client_node *new = malloc(sizeof(client_node));

    new->client_fd = client_fd;
    strcpy(new->username, username);

    pthread_mutex_lock(&list_lock);
    new->next = head;
    head = new;
    pthread_mutex_unlock(&list_lock);
}

void remove_client(int client_fd)
{

    pthread_mutex_lock(&list_lock);
    client_node *curr = head;
    client_node *prev = NULL;

    while (curr != NULL)
    {
        if (curr->client_fd == client_fd)
        {
            if (prev)
                prev->next = curr->next;
            else
                head = curr->next;

            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    pthread_mutex_unlock(&list_lock);
}

int validate_login(package *user_info)
{

    int fd = open("user_info.txt", O_RDONLY);

    lseek(fd, 0, SEEK_SET);

    char ch;
    char buff[100];
    int i;
    int ret;

    int flag1 = 0;
    int flag2 = 0;
    while (1)
    {
        i = 0;
        while ((ret = read(fd, &ch, 1)) >= 1)
        {
            if (ch == '\n')
            {
                break;
            }
            buff[i++] = ch;
        }
        buff[i] = '\0';
        if (ret == 0)
        {
            break;
        }

        char *username = strtok(buff, ",");
        char *password = strtok(NULL, ",");

        if (!username || !password)
            continue;

        if (!strcmp(username, user_info->username))
        {
            flag1 = 1;
            if (!strcmp(password, user_info->password))
            {
                flag2 = 1;
                // close(fd);
                break;
            }
        }
    }

    if (flag1 && flag2)
    {
        close(fd);
        return 1;
    }
    else if (flag1 == 0 && flag2 == 0)
    {
        close(fd);
        return 0;
    }
    else if (flag1 == 1 && flag2 == 0)
    {
        close(fd);
        return 2;
    }
}

int validate_signup(package *user_info)
{

    int fd = open("user_info.txt", O_RDWR | O_APPEND);

    lseek(fd, 0, SEEK_SET);

    char ch;
    char buff[100];
    int i;
    int ret;

    int flag = 0;
    while (1)
    {
        i = 0;
        while ((ret = read(fd, &ch, 1)) >= 1)
        {
            if (ch == '\n')
            {
                break;
            }
            buff[i++] = ch;
        }
        buff[i] = '\0';
        if (ret == 0)
        {
            break;
        }

        char *username = strtok(buff, ",");
        char *password = strtok(NULL, ",");

        if (!username || !password)
            continue;

        if (!strcmp(username, user_info->username))
        {
            close(fd);
            return 0;
        }
    }

    lseek(fd, 0, SEEK_END);
    dprintf(fd, "%s,%s\n", user_info->username, user_info->password);

    close(fd);
    return 1;
}

void send_to_all(int client_fd, msg *ms)
{

    pthread_mutex_lock(&list_lock);
    client_node *curr = head;

    while (curr)
    {
        if (curr->client_fd != client_fd)
        {
            send(curr->client_fd, ms, sizeof(*ms), 0);
        }
        curr = curr->next;
    }

    pthread_mutex_unlock(&list_lock);
}

void send_to_user(char *username, msg *ms)
{
    pthread_mutex_lock(&list_lock);
    client_node *curr = head;

    while (curr)
    {
        if (!strcmp(curr->username, username))
        {
            send(curr->client_fd, ms, sizeof(*ms), 0);
            break;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&list_lock);
}

void *client_thread(void *arg)
{

    client_t *client = (client_t *)arg;

    package user_info;
    recv(client->client_fd, &user_info, sizeof(user_info), 0);

    int ret;
    pthread_mutex_lock(&my_lock);
    if (user_info.option == LOGIN)
    {
        ret = validate_login(&user_info);
    }
    else
    {
        ret = validate_signup(&user_info);
    }
    pthread_mutex_unlock(&my_lock);

    Ack_packet user_ack;
    if (user_info.option == LOGIN)
    {
        if (ret == 0)
        {
            user_ack.option = ERROR;
            strcpy(user_ack.message, "user-name not found");
            send(client->client_fd, &user_ack, sizeof(user_ack), 0);
            // remove_client(client->client_fd);
            close(client->client_fd);
            return NULL;
        }
        else if (ret == 1)
        {
            user_ack.option = SUCCESS;
            strcpy(user_ack.message, "Logged in successfully");
            send(client->client_fd, &user_ack, sizeof(user_ack), 0);
            add_client(client->client_fd, user_info.username);
            send_online_users(client->client_fd);      
            user_online(user_info.username, client->client_fd);
        }
        else if (ret == 2)
        {
            user_ack.option = ERROR;
            strcpy(user_ack.message, "Password not matching");
            send(client->client_fd, &user_ack, sizeof(user_ack), 0);
            // remove_client(client->client_fd);
            close(client->client_fd);
            return NULL;
        }
    }
    else
    {
        if (ret == 0)
        {
            user_ack.option = ERROR;
            strcpy(user_ack.message, "Duplicate user-name");
            send(client->client_fd, &user_ack, sizeof(user_ack), 0);
            // remove_client(client->client_fd);
            close(client->client_fd);
            return NULL;
        }
        else
        {
            user_ack.option = SUCCESS;
            strcpy(user_ack.message, "Registered successfully");
            send(client->client_fd, &user_ack, sizeof(user_ack), 0);
            add_client(client->client_fd, user_info.username);
            send_online_users(client->client_fd);
            user_online(user_info.username, client->client_fd);
        }
    }

    while (1)
    {
        // char msg[512];
        msg m1;
        memset(&m1, 0, sizeof(m1));
        recv(client->client_fd, &m1, sizeof(m1), 0);

        if (!strcmp(m1.message, "EXIT"))
        {
            break;
        }

        // char *check = strtok(msg,":");
        // char *message = strtok(NULL,":");

        if (!strcmp(m1.username, "Group"))
        {
            send_to_all(client->client_fd, &m1);
        }
        else
        {
            char buff[25];
            // snprintf(buff,sizeof(buff),"%s: %s",user_info.username,message);
            strcpy(buff, m1.username);
            strcpy(m1.username, user_info.username);
            send_to_user(buff, &m1);
        }
    }
    user_offline(user_info.username, client->client_fd);
    remove_client(client->client_fd);
    return NULL;
}

int main()
{

    int data_sockfd;
    signal(SIGINT, myhandler);
    signal(SIGTSTP, myhandler);

    pthread_mutex_init(&my_lock, NULL);
    pthread_mutex_init(&list_lock, NULL);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in myaddr;
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(PORT);
    myaddr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(socket_fd, (struct sockaddr *)&myaddr, sizeof(myaddr));
    if (ret == -1)
    {
        perror("bind");
        return 0;
    }

    int ret1 = listen(socket_fd, CLIENT_LEN);

    pthread_t tid[CLIENT_LEN];
    // int i = 0;
    client_t clients[CLIENT_LEN];
    int client_count = 0;
    while (1)
    {

        // struct sockaddr_in client_addr[10];
        // int client_fd[10];
        printf("Listening on port %d ...\n", PORT);
        clients[client_count].client_fd = accept(socket_fd, (struct sockaddr *)NULL, NULL);

        pthread_create(&tid[client_count], NULL, client_thread, &clients[client_count]);

        pthread_detach(tid[client_count]);
        client_count++;
    }

    pthread_mutex_destroy(&my_lock);
    pthread_mutex_destroy(&list_lock);
}