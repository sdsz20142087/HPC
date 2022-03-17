#include "string.h"
#include "common/macro.h"
#include "proto/relation_info_base.pb.h"

using namespace ssp;

class RelationInfo {
public:
	void FromPb(ssp::RelationInfoBase& relation);
	void ToPb(ssp::RelationInfoBase& relation);

private:
	GETSETVAR(int,user_id);
	GETSETVAR(int,friend_count)
	int friend_list_[10240];
	GETSETVAR(int,black_count)
	int black_list_[10240];

public:
	int GetFriendUserIdByIndex(int index);
	int GetBlackUserIdByIndex(int index);
	int CheckFriend(int other_id);
	int CheckBlack(int other_id);
	int AddFriend(int other_id);
	int AddBlack(int other_id);
	int DeleteFriend(int other_id);
	int DeleteBlack(int other_id);
};
