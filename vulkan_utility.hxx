//TODO - rename Vulkan_Utility?
#ifndef VULKAN_UTILITY_HXX
#define VULKAN_UTILITY_HXX

extern "C"{
#include <vulkan/vulkan.h>
}
//VK_NULL_HANDLE for use in function arguments. avoids zero-as-nullptr warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
template<typename T>
constexpr T VK_NULL_HANDLE_ARG = VK_NULL_HANDLE;
#pragma GCC diagnostic pop


//Vulkan Extensions
VkResult CreateDebugReportCallbackEXT(
	VkInstance instance,
	const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugReportCallbackEXT* pCallback
){
	auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>
	(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));

	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

VkResult DestroyDebugReportCallbackEXT(
	VkInstance 					 instance,
	VkDebugReportCallbackEXT  	 callback,
	const VkAllocationCallbacks* pAllocator
){
	auto func = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>
	(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));

	if (func != nullptr) {
		func(instance, callback, pAllocator);
		return VK_SUCCESS;
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

//a basic image has only 1 mipmap level and layer
//TODO: move into Basic_ImageViewCreateInfo?
constexpr VkImageSubresourceRange Basic_Image_Range
(
	VkImageAspectFlags aspectMask
){
	VkImageSubresourceRange image_range = {};
	image_range.aspectMask 	   = aspectMask;
	image_range.baseMipLevel   = 0;
	image_range.levelCount 	   = 1;
	image_range.baseArrayLayer = 0;
	image_range.layerCount 	   = 1;
	return image_range;
}

constexpr VkImageAspectFlags Choose_Aspect_Mask (VkFormat format)
{
	switch(format)
	{
	case VK_FORMAT_R8G8B8A8_UNORM:
	case VK_FORMAT_B8G8R8A8_UNORM:
		return VK_IMAGE_ASPECT_COLOR_BIT;

	case VK_FORMAT_D32_SFLOAT:
		return VK_IMAGE_ASPECT_DEPTH_BIT;
	default:
		return 0xFFFFFFFF;
	}
}

//a basic image is 2D, with 1 mipmap level and 1 layer
constexpr VkImageViewCreateInfo Basic_ImageViewCreateInfo
(
	VkImage image,
	VkFormat format
){
	VkImageViewCreateInfo view_info = {};
	view_info.sType 		   = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image			   = image;
	view_info.viewType 		   = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format 		   = format;
	view_info.subresourceRange = Basic_Image_Range(Choose_Aspect_Mask(format));
	return view_info;
}

//todo - rename to Execute_Commands_Synchronous or so?
//todo - concepts would be cool here
//todo - have commands_func return bool = submit_or_not?
//executes one time command synchronously
//todo - VkCmd** needs command_buffer as arg
//bool (*Commands_Func)(VkCommandBuffer)
template <typename Commands_Func>
VkResult Execute_Commands_Now(
	VkDevice 	  device,
	VkCommandPool command_pool,
	VkQueue 	  command_queue,
	Commands_Func commands_func
){
	//Allocate one command buffer
	VkCommandBuffer command_buffer;
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool		   = command_pool;
	alloc_info.level 			   = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = 1;
	VkResult r = vkAllocateCommandBuffers(
		device,
		&alloc_info,
		&command_buffer
	);
	if(r != VK_SUCCESS) return r;
	//Begin command buffer
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	r = vkBeginCommandBuffer(command_buffer, &begin_info);
	if(r != VK_SUCCESS) return r;
	//Execute provided func - should be mostly vkCmd...
	commands_func(command_buffer);
	//Submit
	r = vkEndCommandBuffer(command_buffer);
	if(r != VK_SUCCESS) return r;
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;
	r = vkQueueSubmit(
		command_queue,
		1,
		&submit_info,
		VK_NULL_HANDLE_ARG<VkFence>
	);
	if(r != VK_SUCCESS) return r;
	r = vkQueueWaitIdle(command_queue);
	if(r != VK_SUCCESS) return r;
	//Cleanup
	vkFreeCommandBuffers(
		device,
		command_pool,
		1,
		&command_buffer
	);
	return r;
}
#endif
/*
VkCommandBuffer Begin_One_Time_Command
(
	VkDevice 	  device,
	VkCommandPool command_pool
){
	VkCommandBuffer command_buffer;
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool		   = command_pool;
	alloc_info.level 			   = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = 1;
	vkAllocateCommandBuffers(
		device,
		&alloc_info,
		&command_buffer
	);
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(command_buffer, &begin_info);
	return command_buffer;
}

VkResult End_One_Time_Command
(
	VkDevice		device,
	VkCommandPool	command_pool,
	VkCommandBuffer command_buffer,
	VkQueue			submit_queue
){
	//End
	VkResult r = vkEndCommandBuffer(command_buffer);
	if(r != VK_SUCCESS) return r;
	//Submit
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;
	r = vkQueueSubmit(
		submit_queue,
		1,
		&submit_info,
		VK_NULL_HANDLE
	);
	if(r != VK_SUCCESS) return r;
	r = vkQueueWaitIdle(submit_queue);
	if(r != VK_SUCCESS) return r;
	//Cleanup
	vkFreeCommandBuffers(
		device,
		command_pool,
		1,
		&command_buffer
	);
	return r;
}
*/
