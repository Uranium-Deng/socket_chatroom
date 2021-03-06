/*************************************************************************
	> File Name: chatroom.h
	> Author: Uranium_Deng
	> Mail: 1927157164@qq.com
	> Created Time: 2022年02月24日 星期四 16时47分57秒
 ************************************************************************/

#ifndef _CHATROOM_H
#define _CHATROOM_H

struct Msg {
    char from[32];
    int flag; 
    // 0: 公聊信息, 1: 私聊信息, 2:client连接信息或来自server的通知信息 
    // 3:用于已在线，server端让client断开连接
    char message[512];
};

struct RecvMsg {
    struct Msg msg;
    int retval;
};

/* 消息发送，向fd对应的文件发送Msg信息 */
int chat_send(struct Msg msg, int fd);

/* 消息接收，返回RecvMsg结构体 */
struct RecvMsg chat_recv(int fd);

#define MAX_CLIENT 512

#endif
