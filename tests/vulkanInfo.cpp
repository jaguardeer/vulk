#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

static auto PrintVulkanResult(VkResult result, const char* extra_message = nullptr) {
	if(extra_message != nullptr) cout << extra_message << endl;
	cout << "(" << result << "): " << endl;
}

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
	cout << "available extensions:" << endl;
	PrintLayerExtensions();
	cout << endl << "available layers:" << endl;
	PrintVulkanLayers();
	return 0;
}
