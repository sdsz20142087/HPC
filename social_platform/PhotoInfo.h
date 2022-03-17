#include "common/macro.h"
#include "proto/photo_info_base.pb.h"

class PhotoInfo {
public:
	void FromPb(ssp::PhotoInfoBase& photo);
	void ToPb(ssp::PhotoInfoBase& photo);
private:
	GETSETVAR(int,user_id)
	GETSETVAR(int,last_publisher)
	GETSETVAR(int,last_publish_time)
	GETSETVAR(int,last_publish_message_id)
};
