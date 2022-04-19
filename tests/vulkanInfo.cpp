#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

#include <el/Instance.hpp>

using std::cout;
using std::endl;
using std::vector;

using namespace engineLibrary;
using namespace engineLibrary::vulkan;

static auto PrintVulkanResult(VkResult result, const char* extra_message = nullptr) {
	if(extra_message != nullptr) cout << extra_message << endl;
	cout << "(" << result << "): " << endl;
}

static auto CreateVulkanInstance() {
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

	// auto result = Instance::Create(instance_info);
	// if(result.error != VK_SUCCESS) PrintVulkanResult(result.error, "failed to create instance");
	// return std::move(result);
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

int main() {
	cout << "creating vulkan instance..." << endl;
	CreateVulkanInstance();
	cout << "printing layer extensions..." << endl;
	PrintLayerExtensions();
}
