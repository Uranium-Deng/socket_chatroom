/*************************************************************************
	> File Name: server.c
	> Author: Uranium_Deng
	> Mail: 1927157164@qq.com
	> Created Time: 2022年02月24日 星期四 16时55分20秒
 ************************************************************************/

#include "../common/head.h"
#include "../common/tcp_server.h"
#include "../common/common.h"
#include "../common/chatroom.h"

struct User {
    char name[32];
    int online;
    pthread_t tid;
    int fd;
};

char *cfg_file = "./server.cfg";

struct User *client;


/* 判断用户name是否在线 */
bool check_online(char *name) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online == 1 && strcmp(client[i].name, name)) {
            return true;
        }
    }
    return false;
}

/* 在User数组中找出并返回一个空闲下标用于存放客户信息 */
int find_free_index() {

}

/* 线程对应的work函数 */
void *work(void *arg) {
    
}


int main() {
    int port, server_listen, fd;
    struct RecvMsg recvmsg;
    struct Msg msg;
    port = atoi(get_value(cfg_file, "SERVER_PORT"));
    client = (struct User *)calloc(MAX_CLIENT, sizeof(struct User));

    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create");
        return 1;
    }

    while (1) {
        if ((fd = accept(server_listen, NULL, NULL)) < 0) {
            perror("accept");
            continue;
        }
        // accept()接受用户连接后，将用户信息添加到User数组中，并开启一个新线程为其服务

        recvmsg = chat_recv(fd);
        if (recvmsg.retval < 0) {
            // 接收用户信息失败，关闭连接
            close(fd);
            continue;
        }
        if (check_online(recvmsg.msg.from)) {
            // 用户已在线，拒绝连接
            
        } else {
            // 登记用户信息并创建线程为之服务
            int idx = find_free_index();
            client[idx].online = 1;
            client[idx].fd = fd;
            strcmp(client[idx].name, recvmsg.msg.from);
            pthread_create(&client[idx].tid, NULL, work, NULL);
        }

    }

    return 0;
}


