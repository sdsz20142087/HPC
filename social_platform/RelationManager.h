#include "common/macro.h"
#include "RelationInfo.h"

class RelationManager {
	private:
		RelationInfo relations_[10240];
		GETSETVAR(int,relation_count);
	public:
		void Start();
		void Proc();
		void Shutdown();
		void Restart();
	public:
		RelationInfo* GetRelation(int user_id);
		int UserRelationInit(int user_id);
		int AddFriend(int user_id,int other_id);
		int DeleteFriend(int user_id,int other_id);
		int AddBlack(int user_id,int other_id);
		int DeleteBlack(int user_id,int other_id);
};
