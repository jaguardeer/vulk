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

using namespace engineLibrary;
using namespace engineLibrary::vulkan;

static auto isDeviceSuitable(const PhysicalDevice device) {
	// device is suitable if it has a family with VK_QUEUE_GRAPHICS_BIT
	// can check that it is capable of drawing to my window
	const auto queueFamilies = device.getQueueFamilyProperties();
	for(auto &&family : queueFamilies) {
		if(family.queueFlags & VK_QUEUE_GRAPHICS_BIT) return true;
	}
	return false;
}

static auto choosePhysicalDevice(const list<PhysicalDevice> devices) {
	for(auto &&device : devices) {
		if(isDeviceSuitable(device)) return device;
	}
	msg("couldn't find a suitable device");
	return PhysicalDevice{VK_NULL_HANDLE};
}

/*
struct deviceRequirements {
	required, optional
	layers...
	extensions...
	queue families...
	features...

	requiredQueueFamilies.every(physDevice.has == true);
};
*/

static auto createDevice() {
	// VkDeviceCreateInfo:
	// queueCreateInfos - one per queue family that is used <= vkGetPhysicalDeviceQueueFamilyProperties
	// enabledLayers - should match instance createInfo <= vkEnumerateDeviceLayerProperties
	// extensions - enabled device extensions <= vkEnumerateDeviceExtensions
	// enabledFeatures - features to enable <= vkGetPhysicalDeviceFeatures
	uint32_t queueIndex = 0; // TODO:
	const std::vector<float> priorities = {1.0f};
	VkDeviceQueueCreateInfo queueInfo {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = queueIndex,
		.queueCount = static_cast<uint32_t>(priorities.size()),
		.pQueuePriorities = priorities.data(),
	};
}

int main() {
	{
		msg("running app...");

		msg("making main window...");
		Window gameWindow;
		gameWindow.initWindow();

		msg("creating vulkan instance...");
		const auto instance = CreateInstance();
		msg("choosing device...");
		auto devices = instance.enumeratePhysicalDevices();
		choosePhysicalDevice(devices.value);


		msg("done!");
		return 0;
		msg("main loop...");
		while(gameWindow.isOpen()) {
			gameWindow.processMessages();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		msg("cleaning up...");
	}
	return 0;
}
