#include "string.h"
#include "common/macro.h"
#include "proto/message_info_base.pb.h"

class MessageInfo {
	GETSETVAR(int,publisher)
	GETSETVAR(int,message_id)
	GETSETSTR(10240,content)
	GETSETVAR(int,publish_time)
};
