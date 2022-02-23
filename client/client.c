/*************************************************************************
	> File Name: client.c
	> Author: Uranium_Deng
	> Mail: 1927157164@qq.com
	> Created Time: 2022年02月23日 星期三 21时55分52秒
 ************************************************************************/

#include "head.h"
#include "tcp_client.h"


int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s ip port!\n", argv[0]);
        return 1;
    }

    int sockfd;
    if ((sockfd = socket_connect(argv[1], atoi(argv[2]))) < 0) {
        perror("socket_connect");
        return 2;
    }

    return 0;
}




