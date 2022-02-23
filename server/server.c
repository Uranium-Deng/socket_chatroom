/*************************************************************************
	> File Name: server.c
	> Author: Uranium_Deng
	> Mail: 1927157164@qq.com
	> Created Time: 2022年02月23日 星期三 21时45分01秒
 ************************************************************************/

#include "head.h"
#include "tcp_server.h"


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

    return 0;
}


