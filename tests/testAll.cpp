// system headers
#include <vulkan/vulkan.h>
#include <iostream>
#include <thread>
#include <vector>
#include <array>

// engineLibrary headers
#include <el/Window.hpp>
#include <el/VkResultString.hpp>
#include <el/Instance.hpp>
#include <el/util.hpp>

// test headers
#include "test-common.hpp"

// add test that counts destructions?

using std::cout;
using std::endl;

using namespace engineLibrary;
using namespace engineLibrary::vulkan;

[[maybe_unused]] static auto PrintVulkanResult (VkResult result, const char* extra_message = nullptr) {
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

int main() {
	{
		msg("running app...");

		PrintVulkanLayers();

		msg("making main window...");
		Window gameWindow;
		gameWindow.initWindow();

		msg("creating vulkan instance...");
		const auto instance = CreateInstance();
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
