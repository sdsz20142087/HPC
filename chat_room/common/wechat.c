/*************************************************************************
	> File Name: wechat.c
	> Author: 
	> Mail: 
	> Created Time: Sat 19 Feb 2022 12:01:10 PM CST
 ************************************************************************/

#include "head.h"
#define SUB_MAX_EVENT 15
extern wechat_usr users[MAX_USRS];

void send_to_all(wechat_msg msg, int fd) {
	for (int i = 0; i < MAX_USRS; i++) {
		if (users[i].is_online) {
			//if (users[i].fd == fd) continue;
			send(users[i].fd, (void *)&msg, sizeof(msg), 0);
		}
	}
	return ;
}

void send_to_private(wechat_msg msg) {
	for (int i = 0; i < MAX_USRS; i++) {
		if (users[i].is_online && !strcmp(users[i].name, msg.to)) {
			send(users[i].fd, (void *)&msg, sizeof(msg), 0);
		}
	}
	return ;
}

int add_to_reactor(int epfd, int fd) {
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = fd;
	//make_nonblock(fd);
	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	wechat_msg msg;
	msg.type = MSGSYS | LOGIN;
	strcpy(msg.name, users[fd].name);
	send_to_all(msg, fd);
	return 1;
}

void* sub_reactor(void* args) {
	int epfd = *(int *)args;
	struct epoll_event event[SUB_MAX_EVENT];
	wechat_msg msg;
	for (;;) {
		int nfds = epoll_wait(epfd, event, SUB_MAX_EVENT, -1);
		if (nfds < 0) perror("epoll_wait");
		for (int i = 0; i < nfds; i++) {
			int fd = event[i].data.fd;
			bzero(&msg, sizeof(msg));
			int ret = recv(fd, (void *)&msg, sizeof(msg), 0);
			if (ret <= 0) DBG(RED"<ret>"NONE"Invalid ret\n");
			if (msg.type & MSGALL) {
				send_to_all(msg, fd);
			} else if (msg.type & MSGPRV) {
				send_to_private(msg);
			} else if (msg.type & SIGNFIN) {
				msg.type = MSGSYS;
				users[i].is_online = 0;
				send_to_all(msg, fd);
			}

			DBG(YELLOW"%s\n"NONE, msg.msg);
		}
	}
}