#include "window_win32.hxx"
#include <windows.h>
#include <iostream>
using namespace std;

namespace {
void OnMouseMoveRaw (LONG x_delta, LONG y_delta) {}
void OnMouseWheelRaw	(USHORT mouse_wheel_movement) {}
void OnKeyDown (WPARAM wparam) {}
void OnKeyUp (WPARAM wparam) {}
void OnMouseMove (LPARAM lparam) {}
}

int main()
{
	Window_Create_Info winfo = {"guccimane",420,600};
	auto window_or_error = window_win32::Create(winfo);
	if(window_or_error.is_left) cout << "success";
	else cout << "fail";
	cout << endl;
	//cout << window_or_error.value.left << endl;
	window_win32 w = window_or_error.value.left;
	Sleep(5000);
	w.show();
	Sleep(5000);
	w.hide();
	Sleep(5000);
	return 0;
}