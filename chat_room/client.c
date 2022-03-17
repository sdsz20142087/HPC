/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: Fri 04 Feb 2022 06:24:39 PM CST
 ************************************************************************/

#include "./common/head.h"
char name[20] = "dongyingzhe";
int sockfd;
WINDOW *msg_win, *sub_msg_win, *info_win, *sub_info_win, *input_win, *sub_input_win;
void* msgrecv(void* args) {
	int fd = *(int *)args;
	wechat_msg msg;
	while (1) {
		memset(&msg, 0, sizeof(msg));
		int ret = recv(fd, (void *)&msg, sizeof(msg), 0);
		putchar(10);
		if (msg.type & MSGALL) {
			DBG(L_BLUE"<info to all : >"NONE"from %s : %s\n", msg.name, msg.msg);
			show_msg(&msg);
		} else if (msg.type & MSGPRV) {
			DBG(L_GREEN"<info to you : >"NONE"from %s : %s\n", msg.name, msg.msg);
			show_msg(&msg);
		} else if (msg.type & MSGSYS) {
			if (msg.type & LOGIN) DBG(PINK"<system info : >"NONE"%s log in\n", msg.name);
			else DBG(PINK"<system info : >"NONE"%s log out\n", msg.name);
			show_msg(&msg);
		}
		//DBG(BLINK"<%s> : "NONE, name);
		// fflush(stdout);
		// sleep(1);
		// DBG(L_PINK"%d\n "NONE, ret);
	}
	return (void *)0;
}

void logout(int signum) {
	wechat_msg msg;
	msg.type = SIGNFIN;
	strcpy(msg.name, name);
	send(sockfd, (void *)&msg, sizeof(msg), 0);
	exit(0);
}


int main(int argc, char** argv) {
	//确定个人信息
	int opt, SERV_PORT = 8000, sex = 0, mode = 1;
	char SERV_IP[20] = "101.201.75.177", passwd[20] = "19990729ww";
	while ((opt = getopt(argc, argv, "h:p:s:n:P:m:")) != -1) {
		switch(opt) {
			case 'h' :
				bzero(SERV_IP, strlen(SERV_IP));
				strcpy(SERV_IP, optarg);
				break;
			case 'p' : SERV_PORT = atoi(optarg); break;
			case 's' : sex = atoi(optarg); break;
			case 'n' :
				bzero(name, strlen(name));
				strcpy(name, optarg);
				break;
			case 'P' :
				bzero(passwd, strlen(passwd));
				strcpy(passwd, optarg);
				break;
			case 'm' : mode = atoi(optarg); break;
			default :
				DBG(RED"Usage : "NONE"%s -p port -P passwd -n name -h host -s sex -m mode\n", argv[0]);
		}
	}
	#ifdef UI
    init_ui();
    #endif
	DBG(PINK"INFO : "NONE"%s %d %s\n", SERV_IP, SERV_PORT, (sex == 0 ? "male" : "female"));
	DBG(PINK"INFO : "NONE"%s %s %s\n", name, passwd, (mode == 0 ? "register" : "login"));
	//连接服务器
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serveraddr;
	char buf[MAX_LENGTH] = "";
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, SERV_IP, &serveraddr.sin_addr);
	connect(sockfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
	//发送注册或者登陆消息
	wechat_msg w_msg;
	bzero(&w_msg, sizeof(w_msg));
	strcpy(w_msg.name, name);
	strcpy(w_msg.passwd, passwd);
	w_msg.sex = sex;
	if (mode == 0) w_msg.type = SIGNUP;
	else w_msg.type = SIGNIN;
	send(sockfd, (void *)&w_msg, sizeof(w_msg), 0);
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(sockfd, &rfds);
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	if (select(sockfd + 1, &rfds, NULL, NULL, &tv) <= 0) DBG(RED"system error\n"NONE);
	int ret = recv(sockfd, (void *)&w_msg, sizeof(w_msg), 0);
	if (ret <= 0) DBG(RED"system error\n"NONE);
	if (w_msg.type & SIGNACK) DBG(GREEN"<SUCCESS>"NONE"sign Successfully\n");
	else DBG(RED"<FAIL>"NONE"sign failed\n");
	DBG(L_CYAN"<RUNNING>"NONE"program running in directed mode\n");
	
	// int n = read(0, buf, MAX_LENGTH);
	// write(sockfd, buf, n);
	// //write(sockfd, buf, n - 1);
	// printf("response from server : \n");
	// n = read(sockfd, buf, MAX_LENGTH);
	// write(1, buf, n);

	pthread_t tid;
	pthread_create(&tid, NULL, msgrecv, (void *)&sockfd);
	DBG(L_GREEN"Please input : \n"NONE);
	signal(SIGINT, logout);
	while(1) {
		DBG(BLINK"<%s> : "NONE, name);
		memset(w_msg.msg, 0, sizeof(w_msg.msg));
		// scanf("%[^\n]s", w_msg.msg);
		// getchar();
		echo();
        nocbreak();
        mvwscanw(input_win, 2, 1, "%[^\n]s", w_msg.msg);
		if (!strlen(w_msg.msg)) continue;
		w_msg.type = MSGALL;
		if (w_msg.msg[0] == '@') {
			w_msg.type = MSGPRV;
			strncpy(w_msg.to, w_msg.msg + 1, strchr(w_msg.msg, ' ') - w_msg.msg - 1);
		}
		send(sockfd, (void *)&w_msg, sizeof(w_msg), 0);
		wclear(input_win);
        box(input_win, 0, 0);
        noecho();
        cbreak();
	}
	close(sockfd);
	return 0;
}