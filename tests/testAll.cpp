#include <vulkan/vulkan.h>
#include <iostream>
#include <thread>
#include <vector>
<<<<<<< HEAD
#include <array>
=======
>>>>>>> 597cb070db415ebb161d158dcd52676c24ab7f3c

// engineLibrary headers
#include <el/Window.hpp>
#include <el/VkResultString.hpp>
#include <el/Instance.hpp>
#include <el/util.hpp>

// TODO: move Instance class into implementation file
// add test that counts destructions?

using std::cout;
using std::endl;

using namespace engineLibrary;
using namespace engineLibrary::vulkan;

static auto PrintVulkanResult(VkResult result, const char* extra_message = nullptr) {
	if(extra_message != nullptr) cout << extra_message << endl;
	cout << "(" << result << "): " << GetVkResultString(result) << endl;
}

// returns list of available layers
static auto GetVulkanLayers () {
	uint32_t numLayers;
	vkEnumerateInstanceLayerProperties(&numLayers, nullptr);
	std::vector<VkLayerProperties> layers(numLayers);
	vkEnumerateInstanceLayerProperties(&numLayers, layers.data());
	return layers;
}

static auto PrintVulkanLayers() {
	for(auto&& layer : GetVulkanLayers()) cout << layer.layerName << endl;
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


int main() {
	auto msg = [](const char* m) { cout << m << endl; };
	{
		msg("running app...");

		PrintVulkanLayers();

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
	}
	msg("done!");
	return 0;
}
