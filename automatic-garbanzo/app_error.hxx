#ifndef APP_ERROR_HXX
#define APP_ERROR_HXX

struct App_Error
{
	const char* error_msg;

	operator bool() const {return error_msg != nullptr;}
};

#endif