/*************************************************************************
	> File Name: tcp_client.c
	> Author: Uranium_Deng
	> Mail: 1927157164@qq.com
	> Created Time: 2022年02月23日 星期三 21时39分01秒
 ************************************************************************/


#include "./head.h"

int socket_connect(char *host, int port) {
    int sockfd;
    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    printf("client socket\n");

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        return -1;
    }
    return sockfd;
}

