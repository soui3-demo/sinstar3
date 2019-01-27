#pragma once
#include <winnt.h> // for security attributes constants
#include <aclapi.h> // for ACL

	class SecurityAttribute {
	private:
		SECURITY_ATTRIBUTES sa;
		void _Init();
	public:
		SecurityAttribute(){ _Init(); }
		~SecurityAttribute();
		SECURITY_ATTRIBUTES *get_attr();
	};
