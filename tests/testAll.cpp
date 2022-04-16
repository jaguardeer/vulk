#include <vulkan/vulkan.h>
#include <iostream>
#include <thread>

// engineLibrary headers
#include <el/Window.hpp>
#include <el/VkResultString.hpp>

using std::cout;
using std::endl;

using namespace engineLibrary;

auto PrintVulkanResult(VkResult result, const char* extra_message = nullptr) {
	if(extra_message != nullptr) cout << extra_message << endl;
	cout << "(" << result << "): " << GetVkResultString(result) << endl;
}

void CreateVulkanInstance() {
	static constexpr VkApplicationInfo app_info = {
		.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName    = "test app",
		.applicationVersion  = VK_MAKE_VERSION(0, 0, 1),
		.pEngineName         = "test engine",
		.engineVersion       = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion          = VK_API_VERSION_1_0,
	};

	// TODO: layers & extensions
	static constexpr VkInstanceCreateInfo instance_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app_info,
	};

	VkInstance vk_instance;
	VkResult result = vkCreateInstance(&instance_info, nullptr, &vk_instance);
	if(result != VK_SUCCESS) PrintVulkanResult(result, "failed to create Vulkan instance");
}

int main() {
	cout << "running app..." << endl;

	cout << "making main window..." << endl;
	Window gameWindow;
	gameWindow.initWindow();

	cout << "creating vulkan instance..." << endl;
	CreateVulkanInstance();

	cout << "main loop..." << endl;
	while(gameWindow.isOpen()) {
		gameWindow.processMessages();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	cout << "done!" << endl;
	return 0;
}
