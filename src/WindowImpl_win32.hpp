#ifndef ENGINE_LIBRARY_WINDOWSTRUCT
#define ENGINE_LIBRARY_WINDOWSTRUCT

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace EngineLibrary {
	class WindowImpl {
		public: // todo: friend class vs public?
		//friend class EngineLibrary::Window;
		// todo: return result instead of void
		void InitWindow();
		bool isOpen();
		void ProcessMessages();
		HWND hwnd = nullptr;
		private:
		void RegisterWindowClass();
		static constexpr auto WINDOW_CLASS_NAME = "elWindowClass";
		static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
	};
}      // namespace EngineLibrary
#endif // ENGINE_LIBRARY_WINDOWSTRUCT
