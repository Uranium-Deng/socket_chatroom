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
int sockfd;
char logfile[50] = {0}; // 记录聊天信息的log文件


void logout(int signalnum) {
    close(sockfd);
    printf("您已退出聊天室!\n");
    exit(1);
}


int main() {
    int port;
    struct Msg msg;
    char ip[20] = {0};
    port = atoi(get_value(client_cfg_file, (char *)"SERVER_PORT"));
    strcpy(ip, get_value(client_cfg_file, (char *)"SERVER_IP"));
    strcpy(logfile, get_value(client_cfg_file, "LOG_FILE"));

    if ((sockfd = socket_connect(ip, port)) < 0) {
        // 连接server失败
        perror("socket_connect");
        return 1;
    }
    strcpy(msg.from, get_value(client_cfg_file, "MY_NAME"));
    msg.flag = 2;
    if (chat_send(msg, sockfd) < 0) {
        // 向server第一次发送信息，供server记录本用户
        return 2;
    }

    struct RecvMsg rmsg = chat_recv(sockfd);
    if (rmsg.retval < 0) {
        fprintf(stderr, "Error!\n");
        return 1;
    }
    printf(L_GREEN"Server: "NONE"%s", rmsg.msg.message);
    if (rmsg.msg.flag == 3) {
        close(sockfd);
        return 1;
    }

    pid_t pid;
    signal(SIGINT, logout);  // 设置logout函数来处理终止信号
    if ((pid = fork()) < 0) {
        perror("fork");
    }
    if (pid == 0) {
        // 子进程，负责消息发送
        sleep(1);
        char c = 'a';
        while (c != EOF) {
            system("clear");
            printf(L_PINK"Please input message: "NONE"\n");
            memset(msg.message, 0, sizeof(msg.message));
            scanf("%[^\n]s", msg.message);
            c = getchar(); 
            msg.flag = 0;
            if (msg.message[0] == '@') {
                msg.flag = 1;
            } else if (msg.message[0] == '#') {
                msg.flag = 4;
            }
            if (!strlen(msg.message)) continue;
            chat_send(msg, sockfd);
        }
        close(sockfd);
    } else {
        // 父进程，负责消息接收
        freopen(logfile, "w", stdout);  // 将stdout重定向到logfile中
        printf(L_GREEN"Server "NONE": %s", rmsg.msg.message);
        fflush(stdout);

        while (1) {
            rmsg = chat_recv(sockfd);
            if (rmsg.retval < 0) {
                printf("Error in Server!\n");
                break;
            }
            if (rmsg.msg.flag == 0) { // 公聊信息
                printf(L_BLUE"%s:"NONE" %s\n", rmsg.msg.from, rmsg.msg.message);
            } else if (rmsg.msg.flag == 1) { // 私聊信息
                printf(L_BLUE"%s:"L_GREEN"*"NONE" %s\n", rmsg.msg.from, rmsg.msg.message);
            } else if (rmsg.msg.flag == 2) {  // 服务器通知信息
                printf(L_YELLOW"通知信息: " NONE "%s\n", rmsg.msg.message);
            } else { // 出错
                printf("Error!\n");
            }
            fflush(stdout);
        }
        wait(NULL);
        close(sockfd);
    }
    return 0;
}


