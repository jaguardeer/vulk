#include <array>
#include <iostream>
#include <el/VkResultString.hpp>
#include <el/Instance.hpp>

namespace {
	using namespace engineLibrary;
	using namespace engineLibrary::vulkan;

	[[maybe_unused]] static auto msg (const char* c) {
		std::cout << c << std::endl;
	}

	[[maybe_unused]] static auto PrintVulkanResult (VkResult result, const char* extra_message = nullptr) {
		if(extra_message != nullptr) std::cout << extra_message << std::endl;
		std::cout << "(" << result << "): " << GetVkResultString(result) << std::endl;
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
		if(error != VK_SUCCESS) {
			msg("failed to create Vulkan Instance: ");
			exit(1);
		}
		return std::move(instance);
	}
}
