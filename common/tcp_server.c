/*************************************************************************
	> File Name: tcp_server.c
	> Author: Uranium_Deng
	> Mail: 1927157164@qq.com
	> Created Time: 2022年02月23日 星期三 21时33分43秒
 ************************************************************************/

#include "head.h"
#include "tcp_server.h"


int socket_create(int port) {
    int server_listen;

    if ((server_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_listen, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(server_listen, 20) < 0) {
        perror("listen");
        return -1;
    }

    return server_listen;
}


