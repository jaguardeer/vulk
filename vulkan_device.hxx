#ifndef VULKAN_DEVICE_HXX
#define VULKAN_DEVICE_HXX
extern "C"{
#include <vulkan/vulkan.h>
}
#include "result.hxx"
namespace Vulkan_Device{

template <typename ResultType>
using Result = engine_library::Result<ResultType, VkResult>;

//TODO - pass CreateInfo by reference or by pointer?
//Vulkan_Device::Device wrapper class of VkDevice
class Device
{
public:
	//Casting to VkDevice
	operator  VkDevice  () const {return _device;}//temp for graphics_engine
	VkDevice* operator& () 		 {return &_device;}//temp for graphics_engine
	Device&   operator= (const VkDevice d) {_device = d; return *this;}//temp

	//CreateXXX and AllocateXXX
	Result<VkImage>
		CreateImage			 (const VkImageCreateInfo *);
	Result<VkBuffer>
		CreateBuffer		 (const VkBufferCreateInfo *);
	Result<VkDeviceMemory>
		AllocateMemory		 (const VkMemoryAllocateInfo *);
	Result<VkImageView>
		CreateImageView		 (const VkImageViewCreateInfo *);
	Result<VkFramebuffer>
		CreateFramebuffer 	 (const VkFramebufferCreateInfo	*);
	Result<VkCommandPool>
		CreateCommandPool	 (const VkCommandPoolCreateInfo *);
	Result<VkDescriptorPool>
		CreateDescriptorPool (const VkDescriptorPoolCreateInfo *);

	Result<VkSampler>
		CreateSampler (const VkSamplerCreateInfo *);
	Result<VkDescriptorSetLayout>
		CreateDescriptorSetLayout (const VkDescriptorSetLayoutCreateInfo *);

	//Other
	VkResult BindBufferMemory (VkBuffer, VkDeviceMemory, VkDeviceSize offset);
	VkMemoryRequirements GetBufferMemoryRequirements (VkBuffer buffer);
	VkMemoryRequirements GetImageMemoryRequirements  (VkImage  image );

private:
	VkDevice _device;
	static constexpr VkAllocationCallbacks* pAllocator = nullptr;
};


}//Vulkan_Device
#endif
