#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

#include "test-common.hpp"
#include <el/Instance.hpp>

using std::cout;
using std::endl;
using std::vector;

// returns list of available extensions
static auto PrintLayerExtensions(const char* layerName = nullptr) {
	uint32_t numExtensions;
	VkResult err;
	err = vkEnumerateInstanceExtensionProperties(layerName, &numExtensions, nullptr);
	if(err != VK_SUCCESS) PrintVulkanResult(err, "failed to query # extensions");
	vector<VkExtensionProperties> extensions(numExtensions);
	err = vkEnumerateInstanceExtensionProperties(layerName, &numExtensions, extensions.data());
	for(const auto &extension : extensions) {
		cout << extension.extensionName << endl;
	}
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
	msg("available extensions:");
	PrintLayerExtensions();
	msg("\navailable layers:");
	PrintVulkanLayers();

	msg("\ncreating instance...");
	auto instance = CreateInstance();
	msg("physical devices:");
	auto [devices, error] = instance.enumeratePhysicalDevices();
	for(auto &&device : devices) {
		msg(device.getProperties().deviceName);
	}
	return 0;
}
