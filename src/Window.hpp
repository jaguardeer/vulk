#ifndef ENGINE_LIBRARY_WINDOW
#define ENGINE_LIBRARY_WINDOW

#ifdef _WIN32
#include "WindowImpl_win32.hpp"
#endif // #ifdef _WIN32
#ifdef __linux__
#include "WindowImpl_linux.hpp"
#endif // #ifdef linux

namespace EngineLibrary{
	class Window {
		// todo: return result instead of void
		public:
			void initWindow();
			bool isOpen() const;
			void processMessages(); // todo: move this elsewhere?
			VulkanSurfaceInfo getSurfaceInfo(); // returns info needed to build the appropriate Vk**SurfaceCreateInfo struct
		private:
			EngineLibrary::WindowImpl _impl;
	};
}      // namespace EngineLibrary
#endif // ENGINE_LIBRARY_WINDOW
