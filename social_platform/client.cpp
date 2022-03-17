#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "common/mess_type.h"
#include "proto/message_define.pb.h"

ssp::RegReq regReq1;
ssp::RegRsp regRsp1;
ssp::LoginReq loginReq1;
ssp::LoginRsp loginRsp1;
ssp::RegReq regReq2;
ssp::RegRsp regRsp2;
ssp::LoginReq loginReq2;
ssp::LoginRsp loginRsp2;
ssp::AddFriendReq addFriendReq1;
ssp::AddFriendRsp addFriendRsp1;
ssp::PublishMessageReq publishReq1;
ssp::PublishMessageRsp publishRsp1;
ssp::GetPhotoReq getPhotoReq2;
ssp::GetPhotoRsp getPhotoRsp2;


int ProtoInit(){
	regReq1.set_ver(1);
	regReq1.set_mess_type(401);
	regReq1.set_user_name("dongyingzhe1");
	regReq1.set_password("12345678");

	regReq2.set_ver(1);
        regReq2.set_mess_type(401);
        regReq2.set_user_name("dongyingzhe2");
        regReq2.set_password("12345678");

	loginReq1.set_ver(1);
	loginReq1.set_mess_type(101);
	loginReq1.set_user_name("dongyingzhe1");
	loginReq1.set_password("12345678");

	loginReq2.set_ver(1);
        loginReq2.set_mess_type(101);
        loginReq2.set_user_name("dongyingzhe2");
        loginReq2.set_password("12345678");

	addFriendReq1.set_ver(1);
	addFriendReq1.set_mess_type(301);
	addFriendReq1.set_user_id(10002);
	addFriendReq1.set_other_id(10003);

	publishReq1.set_user_id(10002);
	publishReq1.set_content("I am here!");

}


unsigned char send_buffer[10244];
unsigned char recv_buffer[10244];
int SetReqMessType(const char * mess_type){
        send_buffer[0]=mess_type[0];
        send_buffer[1]=mess_type[1];
        send_buffer[2]=mess_type[2];
        return 0;
}
int sock;
int SendRegReq1(){
	SetReqMessType("401");
	regReq1.SerializeToArray(send_buffer+3,10240);
	int size_buffer=sizeof(send_buffer);
	int ret=send(sock, send_buffer, size_buffer, 0);
	if(ret>0){
		return 0;
	}
	return -1;
}
int SendRegReq2(){
        SetReqMessType("401");
        regReq2.SerializeToArray(send_buffer+3,10240);
        int size_buffer=sizeof(send_buffer);
        int ret=send(sock, send_buffer, size_buffer, 0);
        if(ret>0){
                return 0;
        }
        return -1;
}

int SendLoginReq1(){
        SetReqMessType("101");
        loginReq1.SerializeToArray(send_buffer+3,10240);
        int size_buffer=sizeof(send_buffer);
        int ret=send(sock, send_buffer, size_buffer, 0);
        if(ret>0){
                return 0;
        }
        return -1;
}

int SendLoginReq2(){
        SetReqMessType("101");
        loginReq2.SerializeToArray(send_buffer+3,10240);
        int size_buffer=sizeof(send_buffer);
        int ret=send(sock, send_buffer, size_buffer, 0);
        if(ret>0){
                return 0;
        }
        return -1;
}

int SendAddFriendReq1(){
        SetReqMessType("301");
        addFriendReq1.SerializeToArray(send_buffer+3,10240);
        int size_buffer=sizeof(send_buffer);
        int ret=send(sock, send_buffer, size_buffer, 0);
        if(ret>0){
                return 0;
        }
        return -1;
}

int SendPublishMessageReq1(){
        SetReqMessType("201");
        publishReq1.SerializeToArray(send_buffer+3,10240);
        int size_buffer=sizeof(send_buffer);
        int ret=send(sock, send_buffer, size_buffer, 0);
        if(ret>0){
                return 0;
        }
        return -1;
}
int mess_type;
int GetMessType(){
	return ( (recv_buffer[0]-'0')*100+(recv_buffer[1]-'0')*10 + recv_buffer[2]-'0');
}

int RecvRsp(){
	int ret=recv(sock,recv_buffer,10244,0);
	if(ret>0){
		int mess_type=GetMessType();
		if(mess_type<0){
			return -1;
		}
		switch(mess_type){
			case PUBLISH_MESSAGE_RSP:
			publishRsp1.ParseFromArray(recv_buffer+3,10240);
			printf("Received PublishMessageRsp:\n");
                        printf("ret      : %d\n", publishRsp1.ret());
			case ADD_FRIEND_RSP:
			addFriendRsp1.ParseFromArray(recv_buffer+3,10240);
			printf("Received AddFriendRsp:\n");
			printf("ret      : %d\n", addFriendRsp1.ret());
			break;
			case LOGIN_RSP:
			loginRsp1.ParseFromArray(recv_buffer+3,10240);
			printf("Received LoginRsp:\n");
			printf("ret	 : %d\n", loginRsp1.ret());
			printf("user_id : %d\n", loginRsp1.user_id());
			break;
			case REG_RSP:
			regRsp1.ParseFromArray(recv_buffer+3,10240);
			printf("Received RegRsp:\n");
			printf("ret	 : %d\n", regRsp1.ret());
			printf("user_id : %d\n", regRsp1.user_id());
			break;
		}
		return 0;
	}

	return -1;
}

int ClientSocketInit(){
	sock=socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;
	memset(&server_addr, 0 ,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(8999);
	int conn_ret=connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(conn_ret<0){
		printf("connect_ret:%d\n",conn_ret);
		return conn_ret;
	}
	return 0;		
}
//
//1.reg hank1
//2.login hank1
//3.reg hank2
//4.login hank2
//5.hank1 add hank2
//6.hank1 publish message
//7.hank2 get photo

int main(){
	int client_on=1;
	int n=0;
	int ret;
	ClientSocketInit();
	while(client_on){
		switch(n){
			case 0:
				ProtoInit();
				n=1;
				break;
			case 1:
				ret=SendRegReq1();
				if(ret==0){
					n++;
				}
				break;
			case 2:
				ret=RecvRsp();
				if(ret==0){
					n++;
				}
				break;
			case 3:
				ret=SendRegReq2();
                                if(ret==0){
                                        n++;
                                }
                                break;
			case 4:
				ret=RecvRsp();
                                if(ret==0){
                                        n++;
				}
				break;
			case 5:
				ret=SendAddFriendReq1();
                                if(ret==0){
                                        n++;
                                }
				break;
			case 6:
				ret=RecvRsp();
                                if(ret==0){
                                        n++;
                                }
				break;
			case 7:
				ret=SendPublishMessageReq1();
                                if(ret==0){
                                        n++;
                                }
                                break;
			case 8:
				ret=RecvRsp();
                                if(ret==0){
                                        n++;
                                }
                                break;
			default:
				client_on=0;
				break;
		}
	}
	return 0;
}
