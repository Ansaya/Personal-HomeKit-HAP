#ifndef HAP_PERMISSION
#define HAP_PERMISSION

namespace hap {

	enum {
		permission_read = 1,
		permission_write = 1 << 1,
		permission_notify = 1 << 2  //Notify = Accessory will notice the controller
	};

}

#endif // !HAP_PERMISSION
