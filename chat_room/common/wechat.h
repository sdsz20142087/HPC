/*************************************************************************
	> File Name: wechat.h
	> Author: 
	> Mail: 
	> Created Time: Fri 18 Feb 2022 01:03:03 PM CST
 ************************************************************************/

#ifndef _WECHAT_H
#define _WECHAT_H
typedef struct wechat_usrs{
	char name[20];
	char passwd[20];
	int sex;
	int fd;
	int is_online;
}wechat_usr;
#define SIGNUP 0x01
#define SIGNIN 0x02
#define SIGNOUT 0x04
#define SIGNACK 0x08
#define MSGALL 0x10
#define MSGPRV 0x20
#define MSGSYS 0x40
#define DELIVER 0x80
#define SIGNFIN 0x100
#define LOGIN 0x200
typedef struct wechat_msg{
	int type;
	int sex;
	char name[20];
	char passwd[20];
	char msg[512];
	char to[20];
}wechat_msg;
int add_to_reactor(int epfd, int fd);
void* sub_reactor(void* args);
#endif
