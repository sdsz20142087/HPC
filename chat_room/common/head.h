/*************************************************************************
	> File Name: head.h
	> Author: 
	> Mail: 
	> Created Time: Fri 18 Feb 2022 12:33:37 AM CST
 ************************************************************************/

#ifndef _HEAD_H
#define _HEAD_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <ncurses.h>
#include <locale.h>
#include "ThreadPool.h"
#include "wechat.h"
#include "color.h"
#include "wechat_ui.h"
#define MAX_LENGTH 5000
#define MAX_USRS 2000
#define MAX_EVENT 10
#ifdef _D
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif
#endif
