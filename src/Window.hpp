#ifndef ENGINE_LIBRARY_WINDOW
#define ENGINE_LIBRARY_WINDOW

#ifdef _WIN32
#include "WindowImpl_win32.hpp"
using windowHandle = HWND;
#endif

namespace EngineLibrary{
	class Window {
		// todo: return result instead of void
		public:
			void InitWindow();
			windowHandle GetHandle();
			bool isOpen();
			void ProcessMessages(); // todo: move this elsewhere?
		private:
			EngineLibrary::WindowImpl _impl;
	};
}      // namespace EngineLibrary
#endif // ENGINE_LIBRARY_WINDOW
