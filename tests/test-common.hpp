#include <array>
#include <iostream>
#include <el/Instance.hpp>

namespace {
	using namespace engineLibrary;
	using namespace engineLibrary::vulkan;

	[[maybe_unused]] static auto msg (const char* c) {
		std::cout << c << std::endl;
	}

	static auto CreateInstance() {
		static constexpr VkApplicationInfo appInfo = {
			.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName    = "test app",
			.applicationVersion  = VK_MAKE_VERSION(0, 0, 1),
			.pEngineName         = "test engine",
			.engineVersion       = VK_MAKE_VERSION(0, 0, 1),
			.apiVersion          = VK_API_VERSION_1_0,
		};

		static constexpr VkInstanceCreateInfo instanceInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &appInfo,
		};

		auto [instance, error] = Instance::Create(instanceInfo);
		if(error == VK_SUCCESS) {
			// print error message...
			exit(1);
		}
		return std::move(instance);
	}
}
