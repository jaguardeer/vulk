#ifndef UTILITY_HPP
#define UTILITY_HPP
extern "C" {
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
}

namespace Input_Win32 {

void Register_RawInput();

bool Key_Is_Down			(int key);
void Process_Message_Queue 	();

LRESULT CALLBACK WindowProc
(
	HWND hwnd,
	UINT msg_id,
	WPARAM wparam,
	LPARAM lparam
);





}
#endif
