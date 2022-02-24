/*************************************************************************
	> File Name: server.c
	> Author: Uranium_Deng
	> Mail: 1927157164@qq.com
	> Created Time: 2022年02月23日 星期三 21时45分01秒
 ************************************************************************/

#include "head.h"
#include "tcp_server.h"

#define MAXCLIENTS 1024

struct Client {
    int flag;
    int fd;
    pthread_t tid;
};

struct Client *client;

/*在client数组中找到一个空位置，存放fd*/
int find_idx() {
    for (int i = 0; i < MAXCLIENTS; i++) {
        if (client[i].flag) continue;
        return i;
    }
    return -1;
}

/*将信息小写字母转为大写字母*/
void getUppercaseMsg(char *msg) {
    for (int i = 0; i < strlen(msg); i++) {
        if (msg[i] >= 'a' && msg[i] <= 'z') {
            msg[i] += 'A' - 'a';
        }
    }
}


void *work(void *arg) {
    int *idx = (int *)arg;
    int fd = client[*idx].fd;
    if (send(fd, "You Are Here", sizeof("You Are Here"), 0) < 0) {
        perror("send");
        close(fd);
        client[*idx].flag = 0;
        return NULL;
    }

    while (1) {
        char msg[512] = {0};
        if (recv(fd, msg, sizeof(msg), 0) <= 0) {
            perror("error in recv function");
            close(fd);
            break;
        }
        printf("Recv: %s\n", msg);
        getUppercaseMsg(msg);
        if (send(fd, msg, sizeof(fd), 0) <= 0) {
            perror("error in send function");
            close(fd);
            break;
        }
    }
    close(fd);
    client[*idx].flag = 0;
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port!\n", argv[0]);
        return 1;
    }

    int port, server_listen;
    port = atoi(argv[1]);

    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create");
        return 2;
    }

    pthread_t tid;
    client = (struct Client *)malloc(sizeof(struct Client) * MAXCLIENTS);
    memset(client, 0, sizeof(client));

    while (1) {
        int fd;
        while ((fd = accept(server_listen, NULL, NULL)) < 0) {
            perror("accept");
        }
        printf("New Client Login\n");
        int idx = -1;
        if ((idx = find_idx()) < 0) {
            fprintf(stderr, "FULL!\n");
            close(fd);
            continue;
        }
        client[idx].flag = 1;
        client[idx].fd = fd;
        pthread_create(&client[idx].tid, NULL, work, (void *)&idx);
    }

    return 0;
}

// ./server 8888

