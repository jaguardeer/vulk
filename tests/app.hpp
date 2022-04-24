#include <iostream>
#include <array>
#include <thread>
// engineLibrary headers
#include <el/Window.hpp>
#include <el/VkResultString.hpp>
#include <el/Instance.hpp>
#include <el/util.hpp>

using namespace engineLibrary;
using namespace engineLibrary::vulkan;

using std::cout, std::endl;

class App {
	public:
	static auto msg (const char* m) { cout << m << endl; }

	App() { msg("running app..."); }
	~App() { msg("cleaning up app..."); }

	static auto PrintVulkanResult(VkResult result, const char* extra_message = nullptr) {
		if(extra_message != nullptr) cout << extra_message << endl;
		cout << "(" << result << "): " << GetVkResultString(result) << endl;
	}

	auto run() {
		msg("making main window...");
		Window gameWindow;
		gameWindow.initWindow();

		msg("creating vulkan instance...");
		const auto [instance, error] = CreateVulkanInstance();
		if(error != VK_SUCCESS) {
			PrintVulkanResult(error, "failed to create vulkan instance");
			return 1;
		}

		msg("main loop...");
		while(gameWindow.isOpen()) {
			gameWindow.processMessages();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		msg("cleaning up...");
		return 0;
	}

	static Result<Instance, VkResult> CreateVulkanInstance() {
		static constexpr VkApplicationInfo app_info = {
			.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName    = "test app",
			.applicationVersion  = VK_MAKE_VERSION(0, 0, 1),
			.pEngineName         = "test engine",
			.engineVersion       = VK_MAKE_VERSION(0, 0, 1),
			.apiVersion          = VK_API_VERSION_1_0,
		};
		static constexpr std::array layerNames = {"VK_LAYER_KHRONOS_validation"};
		// TODO: layers & extensions
		static constexpr VkInstanceCreateInfo instance_info = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &app_info,
			.enabledLayerCount = layerNames.size(),
			.ppEnabledLayerNames = layerNames.data(),
		};
		return Instance::Create(instance_info);
	}
};
