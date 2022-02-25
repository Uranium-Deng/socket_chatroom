/*************************************************************************
	> File Name: client.c
	> Author: Uranium_Deng
	> Mail: 1927157164@qq.com
	> Created Time: 2022年02月25日 星期五 11时29分00秒
 ************************************************************************/

#include "../common/head.h"
#include "../common/chatroom.h"
#include "../common/common.h"
#include "../common/tcp_client.h"
#include "../common/color.h"


char *client_cfg_file = (char *)"./client.cfg";

int main() {
    int port, sockfd;
    struct Msg msg;
    char ip[20] = {0};
    strcpy(ip, get_value(client_cfg_file, (char *)"SERVER_IP"));
    port = atoi(get_value(client_cfg_file, (char *)"SERVER_PORT"));

    if ((sockfd = socket_connect(ip, port)) < 0) {
        perror("socket_connect");
        return 1;
    }
    strcpy(msg.from, get_value(client_cfg_file, "MY_NAME"));
    msg.flag = 2;
    if (chat_send(msg, sockfd) < 0) {
        return 2;
    }

    struct RecvMsg rmsg = chat_recv(sockfd);
    if (rmsg.retval < 0) {
        fprintf(stderr, "Error!\n");
        return 1;
    }
    printf(GREEN"Server: "NONE"%s", rmsg.msg.message);
    if (rmsg.msg.flag == 3) {
        close(sockfd);
        return 1;
    }

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork");
    }
    if (pid == 0) {
        // 子进程，负责消息发送
        while (1) {
            printf(L_PINK"Please input message: "NONE"\n");
            memset(msg.message, 0, sizeof(msg.message));
            scanf("%[^\n]s", msg.message);
            getchar(); 
            chat_send(msg, sockfd);
            system("clear");
        }
    } else {
        // 父进程，负责消息接收
        wait(NULL);
    }

    return 0;
}


