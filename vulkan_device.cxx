#include "vulkan_device.hxx"
using namespace Vulkan_Device;

#define DEFINE_CREATE_FUNC(STRUCT_NAME) 					\
	Result<Vk##STRUCT_NAME> Device::Create##STRUCT_NAME(	\
		const Vk##STRUCT_NAME##CreateInfo* create_info		\
	){														\
		Vk##STRUCT_NAME value;								\
		const auto&& error = vkCreate##STRUCT_NAME(			\
			this->_device	,								\
			create_info		,								\
			this->pAllocator,								\
			&value											\
		);													\
		return {value,error};								\
	}


DEFINE_CREATE_FUNC (Buffer)
DEFINE_CREATE_FUNC (DescriptorPool)
DEFINE_CREATE_FUNC (Framebuffer)
DEFINE_CREATE_FUNC (CommandPool)
DEFINE_CREATE_FUNC (Image)
DEFINE_CREATE_FUNC (ImageView)
DEFINE_CREATE_FUNC (Sampler)
DEFINE_CREATE_FUNC (DescriptorSetLayout)


Result<VkDeviceMemory> Device::AllocateMemory
(
	const VkMemoryAllocateInfo* memory_info
){
	VkDeviceMemory value;
	const auto&& error = vkAllocateMemory(
		this->_device,
		memory_info,
		this->pAllocator,
		&value
	);
	return {value,error};
}

VkResult Device::BindBufferMemory
(
	VkBuffer 	   buffer,
	VkDeviceMemory memory,
	VkDeviceSize   offset
){
	return vkBindBufferMemory(this->_device, buffer, memory, offset);
}

VkMemoryRequirements Device::GetBufferMemoryRequirements (VkBuffer buffer)
{
	VkMemoryRequirements requirements;
	vkGetBufferMemoryRequirements(this->_device, buffer, &requirements);
	return requirements;
}

VkMemoryRequirements Device::GetImageMemoryRequirements (VkImage image)
{
	VkMemoryRequirements requirements;
	vkGetImageMemoryRequirements(this->_device, image, &requirements);
	return requirements;
}


/*
Result<VkDescriptorPool> Device::CreateDescriptorPool(
	const VkDescriptorPoolCreateInfo* descriptor_pool_info
){
	Result<VkDescriptorPool> result;
	result.error = vkCreateDescriptorPool(
		this->_device,
		descriptor_pool_info,
		this->pAllocator,
		&result.value
	);
	return result;
}


Result<VkFramebuffer> Device::CreateFramebuffer(
	const VkFramebufferCreateInfo* framebuffer_info
){
	Result<VkFramebuffer> result;
	result.error = vkCreateFramebuffer(
		this->_device,
		framebuffer_info,
		this->pAllocator,
		&result.value
	);
	return result;
}


Result<VkBuffer> Device::CreateBuffer(
	const VkBufferCreateInfo& buffer_info
){
	Result<VkBuffer> result;
	result.error = vkCreateBuffer(
		this->_device,
		&buffer_info,
		this->pAllocator,
		&result.value
	);
	return result;
}
*/
