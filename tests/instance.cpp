#include <vulkan/vulkan.h>
#include <iostream>

#include <el/Instance.hpp>

using std::cout;
using std::endl;

using namespace engineLibrary;
using namespace engineLibrary::vulkan;

int main() {
	cout << "creating vulkan instance..." << endl;

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

	auto result = Instance::Create(instanceInfo);
	if(result.error == VK_SUCCESS) cout << "success" << endl;
	else cout << "fail" << endl;

	cout << "still alive" << endl;
}

