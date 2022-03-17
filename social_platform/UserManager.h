#include "common/macro.h"
#include "UserInfo.h"
#include "DbManager.h"
class UserManager {

private:
	UserInfo users_[10240];
	GETSETVAR(int,user_count)
	GETSETVAR(int,reg_num)
	GETSETVAR(int,cur_user_id)
	DbManager* db_svr_;

public:
	int Init(DbManager* db_svr);
	int Start();
	int Proc();
	int Shutdown();
	int Restart();

public:
	int ShowAll();
	UserInfo* GetUser(int user_id);
	int CheckExist(int user_id);
	int CreateUser(const char* user_name,const char *pswd, int from, int time_now);
	int DeleteUser(int user_id);
	int SaveUsers();
	int LoginCheck(const char* user_name,const char* user_pswd);
	int UserLogout(int user_id,int now);
	int GetUserIdByUserName(const char* user_name);
	int UpdateUserLoginTime(int user_id, int time_now);
	int UpdateUserFreshTime(int user_id, int time_now);
};

