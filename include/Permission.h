#ifndef HAP_PERMISSION
#define HAP_PERMISSION

namespace hap {

	enum permission {
		permission_read = 1,
		permission_write = 1 << 1,
		permission_notify = 1 << 2,  //Notify = Accessory will notice the controller
		permission_read_notify = permission_read | permission_notify,
		permission_all = permission_read | permission_write | permission_notify
	};

}

#endif // !HAP_PERMISSION
