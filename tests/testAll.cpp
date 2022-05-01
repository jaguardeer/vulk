// system headers
#include <vulkan/vulkan.h>
#include <iostream>
#include <thread>
#include <vector>
#include <array>
#include <algorithm>

// engineLibrary headers
#include <el/Window.hpp>
#include <el/VkResultString.hpp>
#include <el/Instance.hpp>
#include <el/util.hpp>

// test headers
#include "test-common.hpp"

using namespace engineLibrary;
using namespace engineLibrary::vulkan;

struct DeviceRequirements {
	struct Properties {
		list<VkQueueFlagBits> queueFlags;
		//list<const char*> extensions;
		// features...
		// extensions...
		// layers...(depracated?)
		// targetWindow...
	};
	Properties required;
	Properties optional;
};

static bool deviceMeetsRequirements(
		const PhysicalDevice device,
		const DeviceRequirements requirements)
{
	const auto queueFamProperties = device.getQueueFamilyProperties();
	const auto deviceHasQueueFam = [&queueFamProperties](auto requiredFlags) {
		return std::ranges::any_of(queueFamProperties, [=](auto properties) {
				return requiredFlags == (requiredFlags & properties.queueFlags);
				});
	};
	// device is suitable if it has a family with VK_QUEUE_GRAPHICS_BIT
	// should also check that it is capable of drawing to my window
	const auto hasAllQueueFamilies = std::ranges::all_of(
			requirements.required.queueFlags, deviceHasQueueFam);
	return hasAllQueueFamilies;
}

static auto choosePhysicalDevice(const Instance &instance, const DeviceRequirements requirements) {
	const auto [devices, error] = instance.enumeratePhysicalDevices();
	if(error != VK_SUCCESS) {
		msg("failed to enumerate physical devices");
	}
	for(auto &&device : devices) {
		if(deviceMeetsRequirements(device, requirements)) return device;
	}
	msg("couldn't find a suitable device");
	return PhysicalDevice{VK_NULL_HANDLE};
}

struct QueueInfo {
	VkQueueFlags flags;
	list<float>  priorities;
};

static list<VkDeviceQueueCreateInfo> genQueueCreateInfos(
		const PhysicalDevice  physDevice,
		const list<QueueInfo> queueInfos )
{
	const auto getQueueFamIndex = [](const auto props, const auto flags) {
		const auto it = std::ranges::find_if(props, [=](auto prop) { return flags == (prop.queueFlags & flags); });
		return it - props.begin();
	};
	const auto queueFamProps = physDevice.getQueueFamilyProperties();
	list<VkDeviceQueueCreateInfo> output;
	std::ranges::transform(
		queueInfos, std::back_inserter(output), [&](auto info) {
			return VkDeviceQueueCreateInfo {
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = static_cast<uint32_t>(getQueueFamIndex(queueFamProps, info.flags)),
				.queueCount = static_cast<uint32_t>(info.priorities.size()),
				.pQueuePriorities = info.priorities.data()
			};
		}
	);
	for(auto x : output) std::cout << x.flags << std::endl;
	return output;
}

static auto createDevice(const Instance &instance) {
	// VkDeviceCreateInfo:
	// queueCreateInfos - one per queue family that is used <= vkGetPhysicalDeviceQueueFamilyProperties
	// enabledLayers - should match instance createInfo <= vkEnumerateDeviceLayerProperties
	// extensions - enabled device extensions <= vkEnumerateDeviceExtensions
	// enabledFeatures - features to enable <= vkGetPhysicalDeviceFeatures
	const QueueInfo graphicsQueues = {VK_QUEUE_GRAPHICS_BIT, {1.0}};
	const DeviceRequirements requirements = {
		.required = {
			.queueFlags = list<VkQueueFlagBits> {graphicsQueues.flags},
		},
	};
	const auto physDevice = choosePhysicalDevice(instance, requirements);

	const auto queueCreateInfos = genQueueCreateInfos(physDevice, {graphicsQueues});

	return VK_NULL_HANDLE;
}

int main() {
	{
		msg("running app...");

		msg("making main window...");
		Window gameWindow;
		gameWindow.initWindow();

		msg("creating vulkan instance...");
		const auto instance = CreateInstance();
		msg("creating device...");
		const auto device = createDevice(instance);


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
