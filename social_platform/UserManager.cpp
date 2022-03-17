#include <stdio.h>
#include <stdlib.h>
#include "common/ret_value.h"
#include "UserManager.h"
#include "DbManager.h"
#include "proto/user_info_base.pb.h"
int UserManager::LoginCheck(const char* user_name, const char* password){
	for(int i=0;i<user_count_;i++){
		if(strcmp(users_[i].user_name(),user_name)==0){
			if(strcmp(users_[i].password(),password)==0){
				return SUCCESS;
			}else{
				return WRONG_PASSWORD;
			}
		}
	}
	return USER_NOT_EXIST;
}

int UserManager::GetUserIdByUserName(const char* user_name){
	for(int i=0;i<user_count_;i++){
		if(strcmp(users_[i].user_name(),user_name)==0){
			return users_[i].user_id();
		}
	}
	return USER_NOT_EXIST;
}

int UserManager::Init(DbManager* db_svr){
	db_svr_=db_svr;
	printf("UserManager Init\n");
	return SUCCESS;
}

int UserManager::Proc(){
	return SUCCESS;
}

int UserManager::Start(){
	int used_user_id=db_svr_->GetUserId();
	set_cur_user_id(used_user_id);
	int ret=db_svr_->GetUsersBegin();
	int ui=0;
	if(ret==SUCCESS){
		ret=db_svr_->GetUsersOneByOne(&users_[ui]);
		if(ret==SUCCESS)ui++;
		while(ret!=DB_NO_MORE_DATA){
			ret=db_svr_->GetUsersOneByOne(&users_[ui]);
			if(ret==SUCCESS)ui++;
		}
		ret=db_svr_->GetUsersEnd();
		set_user_count(ui);
	}else{
		printf("[Error   ]UserManager Start fail\n");
		return ret;
	}
	ShowAll();
	printf("UserManager Start\n");
	return SUCCESS;
}

int UserManager::Shutdown(){
	SaveUsers();
	printf("UserManager Shutdown\n");
	return SUCCESS;
}

int UserManager::Restart(){
	printf("UserManager Restart\n");
	return SUCCESS;
}

int UserManager::ShowAll(){
	printf("========================================\n");
	for(int i=0;i<user_count();i++){
		printf("| %d | %s | %s |\n",users_[i].user_id(),users_[i].user_name(),users_[i].password());
	}
	printf("========================================\n");
	return 0;
}

UserInfo* UserManager::GetUser(int user_id) {
	for(int i=0;i<user_count_;i++) {
		if(users_[i].user_id() == user_id) {
			return &users_[i];
		}
	}
	return NULL;
}

int UserManager::CreateUser(const char* user_name, const char* pswd,int from,int time_now) {
	for(int i=0;i<user_count_;i++){
		if(strcmp(users_[i].user_name(),user_name)==0){
			printf("USER EXIST %d %s\n",users_[i].user_id(),users_[i].user_name());
			return USER_EXIST;
		}
	}

	if(user_count_<10239){
		printf("DEBUG%d %d\n", user_count_, cur_user_id());
		users_[user_count_].set_user_id(cur_user_id());
		users_[user_count_].set_user_name(user_name);
		users_[user_count_].set_password(pswd);
		users_[user_count_].set_from(from);
		users_[user_count_].set_reg_time(time_now);
		users_[user_count_].set_db_flag(FLAG_INSERT);
		user_count_++;
	}
	set_cur_user_id(cur_user_id()+1);
	set_reg_num(reg_num()+1);
	
	SaveUsers();
	
	return 0;
}

int UserManager::DeleteUser(int user_id){
	for(int i=0;i<user_count_;i++){
		if(users_[i].user_id()==user_id){
			printf("USER EXIST %d %s\n",users_[i].user_id(),users_[i].user_name());
			users_[i].set_db_flag(FLAG_DELETE);
			return USER_EXIST;
		}
	}

	return 0;
}

int UserManager::UpdateUserLoginTime(int user_id,int time_now){
	UserInfo* user=GetUser(user_id);
	if(user==NULL){
		printf("USER_NOT_EXIST %d\n",user_id);
		return USER_NOT_EXIST;
	}
	user->set_login_time(time_now);
	return SUCCESS;
}
int UserManager::UpdateUserFreshTime(int user_id,int time_now){
	UserInfo* user=GetUser(user_id);
	if(user==NULL){
		printf("USER_NOT_EXIST %d\n",user_id);
		return USER_NOT_EXIST;
	}
	user->set_fresh_time(time_now);
	return SUCCESS;
}
int UserManager::UserLogout(int user_id,int now){
	UserInfo* user=GetUser(user_id);
	if(user==NULL){
		return USER_NOT_EXIST;
	}
	user->set_logout_time(now);
	return SUCCESS;
}

int UserManager::CheckExist(int user_id){
	UserInfo* user=GetUser(user_id);
	if(user==NULL){
		printf("USER_NOT_EXIST %d\n",user_id);
		return USER_NOT_EXIST;
	}
	return USER_EXIST;
}

int UserManager::SaveUsers(){
	for(int i=0;i<user_count_;i++){
		/* 1 delete */
		if(users_[i].db_flag()==FLAG_DELETE){
			db_svr_->DeleteUser(&users_[i]);
                        users_[i].set_db_flag(FLAG_INIT);
		}
		/* 2 update */
		if(users_[i].db_flag()==FLAG_UPDATE){
			db_svr_->UpdateUser(&users_[i]);
                        users_[i].set_db_flag(FLAG_INIT);
		}
		/* 3 insert */
		if(users_[i].db_flag()==FLAG_INSERT){
			db_svr_->InsertUser(&users_[i]);
			users_[i].set_db_flag(FLAG_INIT);
		}
	}
	return 0;
}

