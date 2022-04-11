module;
#include <vulkan/vulkan.h>

export module vulkanResultString;

export const char* GetVkResultString(const VkResult vulkan_result);
