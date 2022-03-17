#include <stdio.h>
#include <unistd.h>

#include <sys/time.h>
#include "common/mess_type.h"
#include "common/ret_value.h"
#include "UserManager.h"
#include "RelationManager.h"
#include "MessageManager.h"
#include "PhotoManager.h"
#include "proto/message_define.pb.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

UserManager user_svr;
RelationManager rela_svr;
MessageManager mess_svr;
PhotoManager photo_svr;
DbManager db_svr;
int debug_on=1;

void Test() {
	char user_name[]="Admin";
	char password[]="A12c3d4";
	int ret=user_svr.CreateUser(user_name,password,0,0);
}

unsigned char recv_buffer[10244];
int GetReqMessType(){
	return ( (recv_buffer[0]-'0')*100+(recv_buffer[1]-'0')*10+recv_buffer[2]-'0' );
}
int mess_type = -1;

unsigned char send_buffer[10244];

int server_sock;
struct sockaddr_in server_addr;

int SocketInit() {
	server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(8999);
	bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	listen(server_sock, 10000);
	printf("Socket Inited\n");
}

int no_recv_count=0;
struct sockaddr_in clnt_addr;
int clnt_sock;
int accept_flag=0;
int SocketAccept(){
	struct sockaddr_in clnt_addr;
		socklen_t clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(server_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
	accept_flag=1;
	printf("Socket Accepted\n");
	return 0;
}

int send_fail_count=0;
int ClientClose(){
	send_fail_count=1;
	printf("Client Disconnect and Closed\n");fflush(stdout);
	close(clnt_sock);
	accept_flag=0;
	return 0;
}

int SocketCheckRecv(){
	if(accept_flag==0){
		return 0;
	}
	int ret=recv(clnt_sock,recv_buffer,10244,0);
	if(ret>0){
		mess_type=GetReqMessType();
		if(debug_on)printf("mess_type:%d\n",mess_type);fflush(stdout);
		return 0;
	}else{
		no_recv_count++;
		if(no_recv_count>10){
			ClientClose();
		}
	}

	return -1;
}

int SocketSendRsp(int send_size){
	int ret=send(clnt_sock, send_buffer, send_size, 0);
	if(ret<0){
		ClientClose();
	}
	return 0;
}

int SocketClose(){
	close(server_sock);
	return 0;
}

int SetRspMessType(char *mess_type){
	if(strlen(mess_type)>=3){
		send_buffer[0]=mess_type[0];
		send_buffer[1]=mess_type[1];
		send_buffer[2]=mess_type[2];
	}
	return 0;
}

ssp::RegReq msgRegReq;
ssp::RegRsp msgRegRsp;

ssp::LoginReq msgLoginReq;
ssp::LoginRsp msgLoginRsp;

ssp::LogoutReq logoutReq;
ssp::LogoutRsp logoutRsp;

ssp::PublishMessageReq msgPublishMessageReq;
ssp::PublishMessageRsp msgPublishMessageRsp;

ssp::GetMessageReq msgGetMessageReq;
ssp::GetMessageRsp msgGetMessageRsp;

ssp::GetPhotoReq msgGetPhotoReq;
ssp::GetPhotoRsp msgGetPhotoRsp;

ssp::AddFriendReq msgAddFriendReq;
ssp::AddFriendRsp msgAddFriendRsp;

ssp::DelFriendReq msgDelFriendReq;
ssp::DelFriendRsp msgDelFriendRsp;

ssp::AddBlackReq msgAddBlackReq;
ssp::AddBlackRsp msgAddBlackRsp;

ssp::DelBlackReq msgDelBlackReq;
ssp::DelBlackRsp msgDelBlackRsp;

char mess_type_str[10];

int main() {
	cout << "begin" << endl;
	db_svr.Init();
	user_svr.Init(&db_svr);
	user_svr.Start();
	rela_svr.Start();
	mess_svr.Start();
	photo_svr.Start();
	Test();
	
	SocketInit();
	SocketAccept();
	int times=0;
	int server_on=1;
	while(server_on){
		int now = time(NULL);
		SocketCheckRecv();
		int ret=0;
		if(mess_type<0){
			usleep(5000);
			continue;
		}
		times++;
		if(debug_on)printf("message count: %d\n",times);fflush(stdout);
		switch(mess_type){
			case REG_REQ:
				msgRegReq.ParseFromArray(recv_buffer+3,10240);
				msgRegRsp.set_user_id(0);
				ret=user_svr.CreateUser(msgRegReq.user_name().c_str(),
							msgRegReq.password().c_str(),
							msgRegReq.from(),now);
				msgRegRsp.set_ret(ret);
				if(ret==SUCCESS){
					int user_id=user_svr.GetUserIdByUserName(msgRegReq.user_name().c_str());
					rela_svr.UserRelationInit(user_id);
					msgRegRsp.set_user_id(user_id);
				}
				if(debug_on)printf("[DEBUG   ]RegRsp ret:%d uid:%d\n",ret,msgRegRsp.user_id());fflush(stdout);
				sprintf(mess_type_str,"%d",REG_RSP);
				SetRspMessType(mess_type_str);
				ret=msgRegRsp.SerializeToArray(send_buffer+3,10240);
				SocketSendRsp(msgRegRsp.ByteSize()+3);
				break;
			case LOGIN_REQ:
				msgLoginReq.ParseFromArray(recv_buffer+3,10240);
				if(debug_on)printf("[DEBUG   ]login uname: %s\n",msgLoginReq.user_name().c_str());
				ret=user_svr.LoginCheck(msgLoginReq.user_name().c_str(),
							msgLoginReq.password().c_str());
				if(ret==SUCCESS){
					int user_id=user_svr.GetUserIdByUserName(msgLoginReq.user_name().c_str());
					user_svr.UpdateUserLoginTime(user_id, now);
					msgLoginRsp.set_user_id(user_id);
				}else{
					msgLoginRsp.set_ret(ret);
					msgLoginRsp.set_user_id(0);
				}
				msgLoginRsp.set_ret(ret);
				sprintf(mess_type_str,"%d",mess_type+1);
				SetRspMessType(mess_type_str);
				ret=msgLoginRsp.SerializeToArray(send_buffer+3,10240);
				if(debug_on)if(debug_on)printf("[DEBUG   ]ret:%d\n",ret);
				SocketSendRsp(msgLoginRsp.ByteSize()+3);
				break;
			case ADD_FRIEND_REQ:
				msgAddFriendReq.ParseFromArray(recv_buffer+3,10240);
				if(debug_on)printf("[DEBUG   ]AddFriend, uid1:%d uid2:%d\n",msgAddFriendReq.user_id(),msgAddFriendReq.other_id());
				ret = user_svr.CheckExist(msgAddFriendReq.user_id());
				if(ret==USER_EXIST){
					ret=user_svr.CheckExist(msgAddFriendReq.other_id());
					if(ret==USER_EXIST){
						ret=rela_svr.AddFriend(	msgAddFriendReq.user_id(),
												msgAddFriendReq.other_id());
												msgAddFriendRsp.set_ret(ret);
					}
					msgAddFriendRsp.set_ret(ret);
				}else{
					msgAddFriendRsp.set_ret(ret);
				}
				if(debug_on)printf("[DEBUG   ]302 AddFriendRsp ret:%d\n",ret);fflush(stdout);
				sprintf(mess_type_str,"%d",ADD_FRIEND_RSP);
				SetRspMessType(mess_type_str);
				ret=msgAddFriendRsp.SerializeToArray(send_buffer+3,10240);
				if(debug_on)printf("[DEBUG   ]ret_byte_size:%d\n",ret);
				SocketSendRsp(msgAddFriendRsp.ByteSize()+3);
				break;
			case DEL_FRIEND_REQ:
				msgDelFriendReq.ParseFromArray(recv_buffer+3,10240);
				if(debug_on)printf("[DEBUG   ]mess_type:303, uid1:%d uid2:%d\n",msgDelFriendReq.user_id(),msgDelFriendReq.other_id());
				ret = user_svr.CheckExist(msgDelFriendReq.user_id());
				if(ret==USER_EXIST){
					ret=user_svr.CheckExist(msgDelFriendReq.other_id());
					if(ret==USER_EXIST){
						ret=rela_svr.DeleteFriend(	msgDelFriendReq.user_id(),
									msgDelFriendReq.other_id());
						msgDelFriendRsp.set_ret(ret);
					}else{
						msgDelFriendRsp.set_ret(ret);
					}
				}else{
					msgDelFriendRsp.set_ret(ret);
				}
				if(debug_on)printf("[DEBUG   ]304 DelFriendRsp ret:%d\n",ret);fflush(stdout);
				sprintf(mess_type_str,"%d",DEL_FRIEND_RSP);
				SetRspMessType(mess_type_str);
				ret=msgDelFriendRsp.SerializeToArray(send_buffer+3,10240);
				if(debug_on)printf("[DEBUG   ]ret_byte_size:%d\n",ret);
				SocketSendRsp(msgDelFriendRsp.ByteSize()+3);
				break;
			case ADD_BLACK_REQ:
					msgAddBlackReq.ParseFromArray(recv_buffer+3,10240);
					ret = user_svr.CheckExist(msgAddBlackReq.user_id());
					if(ret==USER_EXIST){
						ret=user_svr.CheckExist(msgAddBlackReq.other_id());
						if(ret==USER_EXIST){
							ret=rela_svr.AddBlack(	msgAddBlackReq.user_id(),
										msgAddBlackReq.other_id());
										msgAddBlackRsp.set_ret(ret);
						}
						msgAddBlackRsp.set_ret(ret);
					}else{
						msgAddBlackRsp.set_ret(ret);
					}
					msgAddBlackRsp.set_ret(ret);
					sprintf(mess_type_str,"%d",mess_type+1);
					SetRspMessType(mess_type_str);
					msgAddBlackRsp.SerializeToArray(send_buffer+3,10240);
					SocketSendRsp(msgAddBlackRsp.ByteSize());
				break;
			case PUBLISH_MESSAGE_REQ:
				msgPublishMessageReq.ParseFromArray(recv_buffer+3,10240);
				ret = user_svr.CheckExist(msgPublishMessageReq.user_id());
				if(ret==USER_EXIST){
					MessageInfo message;
					message.set_message_id(1);/* todo get from mysql */
					message.set_publisher(msgPublishMessageReq.user_id());
					message.set_publish_time(now);
					ret = mess_svr.PublishMessage(message);
					msgPublishMessageRsp.set_ret(ret);
				}else{
					msgPublishMessageRsp.set_ret(ret);
				}
				sprintf(mess_type_str,"%d",mess_type+1);
				SetRspMessType(mess_type_str);
				msgPublishMessageRsp.SerializeToArray(send_buffer+3,10240);
				SocketSendRsp(msgPublishMessageRsp.ByteSize());
				break;
			default:
				if(debug_on)printf("unknown mess_type: %d\n",mess_type);
				break;
		}
		user_svr.Proc();
		rela_svr.Proc();
		mess_svr.Proc();
		photo_svr.Proc();
		mess_type=-1;
		/* 5ms */
		/* usleep(5000); */
	}

	SocketClose();
	photo_svr.Shutdown();
	mess_svr.Shutdown();
	user_svr.Shutdown();
	rela_svr.Shutdown();

	return 0;
}
