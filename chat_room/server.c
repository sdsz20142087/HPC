/*************************************************************************
	> File Name: epo_server.c
	> Author: 
	> Mail: 
	> Created Time: Wed 09 Feb 2022 06:31:53 PM CST
 ************************************************************************/

#include "./common/head.h"
//#define MAX_USRS 1024
wechat_usr users[MAX_USRS];
int epfd, epfd0, epfd1;
char data[MAX_USRS][MAX_LENGTH];
pthread_mutex_t mutex[MAX_USRS];

void* up_server(void* arg) {
	pthread_detach(pthread_self());
	char buf[MAX_LENGTH];
	taskqueue* tq = (taskqueue*)arg;
	while (1) {
		int connfd = TaskQueue_pop(tq);
		pthread_mutex_lock(&mutex[connfd]);
		int n = read(connfd, buf, MAX_LENGTH), ind = strlen(data[connfd]);
		if (!n) {
			epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL);
			close(connfd);
		}
		for (int i = 0 ; i < n; i++) {
			if (buf[i] >= 'a' && buf[i] <= 'z') data[connfd][ind++] = buf[i] - 32;
			else if (buf[i] >= 'A' && buf[i] <= 'Z') data[connfd][ind++] = buf[i] + 32;
			else {
				data[connfd][ind++] = buf[i];
				if (buf[i] == '\n') {
					write(connfd, data[connfd], ind);
					bzero(data[connfd], ind);
				}
				
			}
		}
		pthread_mutex_unlock(&mutex[connfd]);
	}
	return (void *) 0;
}

int main(int argc, char** argv) {
	int listenfd, connfd;
	int SERV_PORT = 8000;
	if (argc > 1) SERV_PORT = atoi(argv[1]);
	struct sockaddr_in serveraddr, clientaddr;
	socklen_t clientlen;
	char info[INET_ADDRSTRLEN];
	taskqueue* tq = (taskqueue *)malloc(sizeof(taskqueue));
	TaskQueue_init(tq, 1000);
	pthread_t tid[5], tid0, tid1;
	for (int i = 0; i < 4; i++) {
		pthread_create(&tid[i], NULL, up_server, (void*)tq);
		//printf("new thread id = %#lx\n", tid[i]);
	}
	
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) perror("socket");
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERV_PORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) perror("bind");
	if (listen(listenfd, 150) == -1) perror("listen");
	for (int i = 0; i < MAX_USRS; i++) pthread_mutex_init(&mutex[i], NULL);

	//主反应堆，监听新用户的到来
	if ((epfd = epoll_create(1)) == -1) perror("epoll_create");
	if ((epfd0 = epoll_create(1)) == -1) perror("epoll_create");
	if ((epfd1 = epoll_create(1)) == -1) perror("epoll_create");

	struct epoll_event ev, event[MAX_EVENT];
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = listenfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
	pthread_create(&tid0, NULL, sub_reactor, (void*)&epfd0);
	pthread_create(&tid1, NULL, sub_reactor, (void*)&epfd1);
	DBG(L_PINK"<ACCEPTING>"NONE"Accepting connection ... \n");
	while (1) {
		int nfds = epoll_wait(epfd, event, MAX_EVENT, -1);
		if (nfds < 0) perror("epoll_wait");
		for (int i = 0; i < nfds; i++) {
			int fd = event[i].data.fd;
			if (fd == listenfd) {
				clientlen = sizeof(clientaddr);
				connfd = accept(listenfd, (struct sockaddr*) &clientaddr, &clientlen);
				if (connfd < 0) perror("accept");
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = connfd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
			} else if (event[i].events & EPOLLIN){
				wechat_msg msg;
				int ret = recv(fd, (void *)&msg, sizeof(msg), 0);
				// if (ret <= 0) {
				// 	epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
				// 	close(fd);
				// }
				// if (ret != sizeof(msg)) DBG(RED"ret_size error\n"NONE);
				if (msg.type & SIGNUP) {
					DBG(YELLOW"SIGN UP OPT\n"NONE);
				} else if (msg.type & SIGNIN) {
					DBG(YELLOW"SIGN IN OPT\n"NONE);
				}
				msg.type = SIGNACK;
				epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
				send(fd, (void *)&msg, sizeof(msg), 0);
				int temp_fd = msg.sex ? epfd1 : epfd0;
				users[fd].is_online = 1;
				users[fd].sex = msg.sex;
				strcpy(users[fd].name, msg.name);
				strcpy(users[fd].passwd, msg.passwd);
				users[fd].fd = fd;
				add_to_reactor(temp_fd, fd);
				
				// int task_id = event[i].data.fd;
				// TaskQueue_push(tq, task_id);
			}
		}
		// pid_t pid = fork();
		// if (pid > 0) {
		// 	close(connfd);
		// 	while (waitpid(-1, NULL, WNOHANG) > 0){}
		// }

		// close(listenfd);

		// int n = read(connfd, buf, MAX_LENGTH);
		// for (int i = 0 ; i < n; i++) buf[i] = toupper(buf[i]);
		// write(connfd, buf, n);
		// close(connfd);
	}
	close(listenfd);
	free(tq);
	return 0;
}
