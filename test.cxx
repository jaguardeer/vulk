// WINDOWS HEADER DEFINES
#define WIN32_LEAN_AND_MEAN
#define STRICT // stricter handle types for Windows API
//#define WINVER _WIN32_WINNT_WIN7
//#define _WIN32_WINNT _WIN32_WINNT_WIN7
//#define NTDDI NTDDI_WIN7 // If you define NTDDI_VERSION, you must also define _WIN32_WINNT.
/*
 * (from windows.h)
 *  If defined, the following flags inhibit definition
 *     of the indicated items.
 */
#define NOMINMAX          // - Macros min(a,b) and max(a,b)

#include <Windows.h>
#include <iostream>
using std::cout;
using std::endl;

auto PrintWindowsError(const char* extra_message);
auto CreateGameWindow();

auto PrintWindowsError(const char* extra_message) {
	const auto error_code = GetLastError();
	LPTSTR error_msg;
	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,// [in]           DWORD   dwFlags,
			nullptr,                                                    // [in, optional] LPCVOID lpSource,
			error_code,                                                 // [in]           DWORD   dwMessageId,
			0,                                                          // [in]           DWORD   dwLanguageId,
			reinterpret_cast<LPTSTR>(&error_msg),                       // [out]          LPTSTR  lpBuffer,
			0,                                                          // [in]           DWORD   nSize,
			nullptr                                                     // [in, optional] va_list *Arguments
			);
	cout << extra_message << endl;
	cout << "(" << error_code << "): " << error_msg << endl;
	LocalFree(error_msg);
}

auto CreateGameWindow() {
	constexpr auto WINDOW_CLASS_NAME = "elWindowClass";
	const auto hInstance = GetModuleHandle(nullptr); // todo - research this
	auto hwnd = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,           // [in]           DWORD     dwExStyle,
			WINDOW_CLASS_NAME,                                  // [in, optional] LPCSTR    lpClassName,
			"Test Window",                                      // [in, optional] LPCSTR    lpWindowName,
			WS_CAPTION | WS_OVERLAPPEDWINDOW | WS_VISIBLE,      // [in]           DWORD     dwStyle,
            CW_USEDEFAULT,                          			// [in]           int       X,
            CW_USEDEFAULT,                          			// [in]           int       Y,
            1024,                                   			// [in]           int       nWidth,
            768,                                    			// [in]           int       nHeight,
            nullptr,                                   			// [in, optional] HWND      hWndParent,
            nullptr,                                   			// [in, optional] HMENU     hMenu,
            hInstance,                              			// [in, optional] HINSTANCE hInstance,
            nullptr                                    			// [in, optional] LPVOID    lpParam
			);

	if(hwnd == nullptr) {
		PrintWindowsError("failed to create game window: ");
	}
}

int main() {
	cout << "running app..." << endl;
	CreateGameWindow();
	return 0;
}
