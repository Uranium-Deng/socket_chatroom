/*************************************************************************
	> File Name: chatroom.c
	> Author: Uranium_Deng
	> Mail: 1927157164@qq.com
	> Created Time: 2022年02月24日 星期四 17时22分10秒
 ************************************************************************/

#include "head.h"
#include "chatroom.h"

int chat_send(struct Msg msg, int fd) {
    if (send(fd, (void *)&msg, sizeof(msg), 0) <= 0) {
        return -1;
    }
    return 0;
}

struct RecvMsg chat_recv(int fd) {
    struct RecvMsg ans;
    memset(&ans, 0, sizeof(ans));
    if (recv(fd, &ans.msg, sizeof(struct Msg), 0) <= 0) {
        ans.retval = -1;
    }
    return ans;
}

