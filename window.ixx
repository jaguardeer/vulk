module;

/*********************
*                    *
*  WINDOWS SPECIFIC  *
*                    *
*********************/

#ifdef _WIN32

#include <Windows.h>

struct OS_Members{
	auto RegisterWindowClass();
	HWND hwnd;
	static constexpr auto WINDOW_CLASS_NAME = "elWindowClass";
};

#endif
#ifdef __linux__

#include <something>

struct OS_Vars{
	HWND hwnd;
};

#endif

export module EngineLibrary.Window;

class Window {
	public:
	auto CreateWindow();
	auto GetHandle() {return _hwnd;}
	auto isOpen();
	auto ProcessMessages();

	private:
	OS_Vars os_vars;
};

