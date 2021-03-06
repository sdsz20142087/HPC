#ifndef _USER_INFO_H_
#define _USER_INFO_H_
#include "string.h"
#include "common/macro.h"
#include "proto/user_info_base.pb.h"

using namespace ssp;

class UserInfo {
public:
	void FromPb(ssp::UserInfoBase& user);
	void ToPb(ssp::UserInfoBase& user);
private:
	GETSETVAR(int,user_id)
	GETSETSTR(256,user_name)
	GETSETSTR(256,nick_name)
	GETSETVAR(int,reg_time)
	GETSETVAR(int,from)
	GETSETVAR(int,login_time)
	GETSETVAR(int,last_login_time)
	GETSETVAR(int,fresh_time)
	GETSETSTR(256,password)
	GETSETVAR(int,db_flag)
	GETSETVAR(int,logout_time)
};

#endif
