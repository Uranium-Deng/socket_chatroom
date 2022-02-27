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
#include "../common/color.h"


struct User {
    char name[32];
    int online;
    pthread_t tid;
    int fd;
};

char *cfg_file = "./server.cfg";
int online_cnts;  // 记录在线总人数
struct User *client;


/* 判断用户name是否在线 */
bool check_online(char *name) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online == 1 && strcmp(client[i].name, name)) {
            printf(YELLOW"W"NONE": %s is online\n", name);
            return true;
        }
    }
    return false;
}

/* 在User数组中找出并返回一个空闲下标用于存放客户信息 */
int find_free_index() {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (!client[i].online) return i;
    }
    return -1;
}

/* 向所有在线用户发送信息 */
void send_all(struct Msg msg) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online != 1) continue;
        chat_send(msg, client[i].fd);
    }
}

/* 向除自己以外的所有在线用户发送信息 */
void send_all_except_me(struct Msg msg, int idx) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online != 1) continue;
        if (idx = i) continue;
        chat_send(msg, client[i].fd);
    }
}

/* 判断该私聊对象是否存在且在线，存在且在线则返回数组下标，否则返回-1 */
int check_name(char *name) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online && !strcmp(client[i].name, name))
            return i;
    }
    return -1;
}

/* 获取当前在线的一部分client姓名 */
void get_online_client(char *message) {
    int cnt = 0;
    char temp[35] = {0};
    sprintf(message, "当前有 ");
    for (int i = 0; i < MAX_CLIENT; i++) {
        if(client[i].online) {
            if (cnt != 0 ) sprintf(temp, ",%s", client[i].name);
            else sprintf(temp, "%s", client[i].name);
            strcat(message, temp);
            cnt++;
            if (cnt >= 5) break;
        } 
    }
    sprintf(temp, " 等%d个用户在线", online_cnts);
    strcat(message, temp);
}


/* 线程对应的work函数,服务端任务：接收+转发*/
void *work(void *arg) {
    int idx = *(int *)arg;
    int client_fd = client[idx].fd;
    
    struct RecvMsg rmsg;
    printf(GREEN"Login "NONE": %s\n", client[idx].name);
    rmsg.msg.flag = 2; // 系统通知信息2
    sprintf(rmsg.msg.message, "你的好友 %s 上线了，和他打个招呼吧!!!", client[idx].name);
    send_all_except_me(rmsg.msg, idx);

    while (1) {
        rmsg = chat_recv(client_fd);
        if (rmsg.retval < 0) { // client下线
            printf(PINK"Logout: "NONE"%s\n", client[idx].name);
            close(client_fd);
            client[idx].online = 0;
            online_cnts--;
            sprintf(rmsg.msg.message, "好友 %s 已下线.", client[idx].name);
            rmsg.msg.flag = 2;
            send_all(rmsg.msg);
            return NULL;
        }

        if (rmsg.msg.flag == 0) {  // 公聊信息0
            printf(BLUE"%s"NONE" : %s\n",rmsg.msg.from, rmsg.msg.message);
            if (!strlen(rmsg.msg.message)) continue;
            send_all(rmsg.msg);
        } else if (rmsg.msg.flag == 1) {  // 私聊信息1
            if (rmsg.msg.message[0] == '@') {
                // 获取私聊对象name
                char to[32] = {0};
                int i = 1;
                for (; i <= 32; i++) {
                    if (rmsg.msg.message[i] == ' ')
                        break;
                }
                strncpy(to, rmsg.msg.message + 1, i - 1);
                // 判断私聊对象是否在线，若不在则告知client私聊对象不在线
                int ret = 0;
                if ((ret = check_name(to)) < 0) {
                    sprintf(rmsg.msg.message, "%s is not online.", to);
                    rmsg.msg.flag = 2; 
                    chat_send(rmsg.msg, client_fd);
                    continue;
                } else if (!strlen(rmsg.msg.message + i)) { // 私聊信息为空
                    sprintf(rmsg.msg.message, "私聊消息不能为空");
                    rmsg.msg.flag = 2;
                    chat_send(rmsg.msg, client_fd);
                    continue;
                }
                printf(L_PINK"Note: "NONE" %s 给 %s 发送了一条私密信息\n", rmsg.msg.from, to);
                chat_send(rmsg.msg, client[idx].fd);
            }
        } else if (rmsg.msg.flag == 4 && rmsg.msg.message[0] == '#') {
            printf(L_PINK"Note: "NONE" %s查询了在线人数\n", rmsg.msg.from);
            get_online_client(rmsg.msg.message);
            rmsg.msg.flag = 2;
            chat_send(rmsg.msg, client_fd);                               
        }
    }
    return NULL;
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
            // 用户已在线，拒绝连接，并告知client端
            msg.flag = 3;
            strcpy(msg.message, "You have Already Login in!\n");
            chat_send(msg, fd);
            close(fd);
            continue;
        }
        msg.flag = 2;
        strcpy(msg.message, "Welcome to this Chatroom!\n");
        chat_send(msg, fd);

        // 登记用户信息并创建线程为之服务
        int idx = find_free_index();
        client[idx].online = 1;
        client[idx].fd = fd;
        online_cnts++;
        strcmp(client[idx].name, recvmsg.msg.from);
        int pthread_ret = pthread_create(&client[idx].tid, NULL, work, (void *)&idx);
        if (pthread_ret) {
            fprintf(stderr, "pthread_create");
        }
    }
    return 0;
}


