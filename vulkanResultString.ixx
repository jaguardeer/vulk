module;
#include <vulkan/vulkan.h>
#include <string>

export module vulkanResultString;

export constexpr const char* GetVkResultString(const VkResult vulkan_result);
