#include <vulkan/vulkan.h>
#include <iostream>
#include <thread>
using std::cout;
using std::endl;
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


/*
 *		VULKAN SECTION
 */

auto PrintVulkanResult(VkResult result, const char* extra_message = nullptr) {
	if(extra_message != nullptr) cout << extra_message << endl;
	cout << "(" << result << "): " << endl;
}

void CreateVulkanInstance() {
	static constexpr VkApplicationInfo app_info = {
		.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName    = "test app",
		.applicationVersion  = VK_MAKE_VERSION(0, 0, 1),
		.pEngineName         = "test engine",
		.engineVersion       = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion          = VK_API_VERSION_1_0
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

	cout << "init..." << endl;
	GameWindow gameWindow;
	gameWindow.CreateGameWindow();
	CreateVulkanInstance();

	cout << "main loop..." << endl;
	while(gameWindow.isOpen()) {
		gameWindow.ProcessMessages();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return 0;
}
