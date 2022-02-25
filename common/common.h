/*************************************************************************
	> File Name: common.h
	> Author: Uranium_Deng
	> Mail: 1927157164@qq.com
	> Created Time: 2022年02月24日 星期四 16时43分57秒
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

/* 从配置文件中获取IP地址或port端口 */
extern char conf_ans[50];
char *get_value(char *path, char *key);

/* 设置非阻塞IO */
void make_nonblock_ioctl(int fd);
void make_block_ioctl(int fd);
void make_nonblock(int fd);
void make_block(int fd);

#endif
