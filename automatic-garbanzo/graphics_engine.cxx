#define VK_USE_PLATFORM_WIN32_KHR
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "graphics_engine.hxx"
//#include "vulkan_setup.hxx"
#include "VkResult_to_CString.hxx"
#include "vulkan_utility.hxx"
#include "minmax.hxx"
#include "window_win32.hxx"
#include <iostream>
#include "renderer.hxx"

using engine_library::darray;
using engine_library::carray;
using engine_library::min;
using engine_library::max;
using namespace Graphics;
using namespace Renderer;//TODO


//Vulkan Environment - Instance Creation

constexpr VkApplicationInfo application_info =
{
	VK_STRUCTURE_TYPE_APPLICATION_INFO,	//	sType
	nullptr,							//	pNext;
	"test_app",							//	pApplicationName
	VK_MAKE_VERSION(0, 0, 0),			//	applicationVersion;
	"test_engine",						//	pEngineName
	VK_MAKE_VERSION(0, 0, 0),			//	engineVersion
	VK_API_VERSION_1_0					//	apiVersion
};

constexpr const char* needed_instance_extensions[] =
{
	"VK_KHR_surface",		//mandatory for drawing to a window
	"VK_KHR_win32_surface",	//os-specific window extension
	"VK_EXT_debug_report",	//optional
};

constexpr const char* needed_layers[] =
{
	"VK_LAYER_KHRONOS_validation"	//optional. older SDKS used VK_LAYER_LUNARG_standard_validation
};


VkResult Graphics_Engine::Create_Vulkan_Instance ()
{
	VkInstanceCreateInfo instance_info = {};
	instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pNext 					= nullptr;
	instance_info.flags 					= 0x0;
	instance_info.pApplicationInfo 			= &application_info;
	instance_info.enabledLayerCount 		= 1;
	instance_info.ppEnabledLayerNames		= needed_layers;
	instance_info.enabledExtensionCount		= 3;
	instance_info.ppEnabledExtensionNames	= needed_instance_extensions;

	VkResult r = vkCreateInstance(
		&instance_info,
		nullptr,
		&this->vulkan_instance
	);

	return r;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback_func
(
	VkDebugReportFlagsEXT 		flags,
	VkDebugReportObjectTypeEXT 	objType,
	uint64_t 					obj,
	size_t 						location,
	int32_t 					code,
	const char* 				layerPrefix,
	const char* 				msg,
	void* 						userData
){
	std::cout << layerPrefix << " : " << msg << std::endl;
	std::cout.flush();
	return VK_FALSE;
}


VkResult Graphics_Engine::Setup_Debug_Callback ()
{
	VkDebugReportCallbackCreateInfoEXT debug_info = {};
	debug_info.sType = 
	VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debug_info.flags =	VK_DEBUG_REPORT_ERROR_BIT_EXT |
	 					VK_DEBUG_REPORT_WARNING_BIT_EXT;
	debug_info.pfnCallback = debug_callback_func;

	//fire away
	return CreateDebugReportCallbackEXT(
		this->vulkan_instance,
		&debug_info,
		nullptr,
		&this->debug_callback
	);
}

using OS_Window_Handle = HWND;
struct Graphics::Graphics_Init_Info::Window_Handle
{
	OS_Window_Handle value;
};
VkResult Graphics_Engine::Create_Window_Surface
(
	Graphics_Init_Info::Window_Handle* handle_ptr
){
	VkWin32SurfaceCreateInfoKHR surface_info = {};
	surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surface_info.pNext = nullptr;
	surface_info.flags = 0x0;
	surface_info.hinstance 	= GetModuleHandle(nullptr);
	surface_info.hwnd 		= handle_ptr->value;

	return vkCreateWin32SurfaceKHR(
		this->vulkan_instance,
		&surface_info,
		nullptr,
		&this->window_surface
	);
}


//Vulkan Environment - Logical Device Creation
//						- Physical Device Selection
//						- Queue Selection

darray<VkPhysicalDevice> Get_Physical_Devices(VkInstance vulkan_instance)
{
	uint32_t num_devices = 0;
	vkEnumeratePhysicalDevices(vulkan_instance, &num_devices, nullptr);
	darray<VkPhysicalDevice> physical_devices(num_devices);
	vkEnumeratePhysicalDevices(
		vulkan_instance,
		&num_devices,
		&physical_devices[0]
	);
	return physical_devices;
}


bool is_device_suitable (VkPhysicalDevice device)
{
	return true;
}


VkPhysicalDevice Graphics_Engine::Pick_Physical_Device ()
{
	auto physical_devices = Get_Physical_Devices (vulkan_instance);

	for(auto&& candidate : physical_devices)
	if( is_device_suitable(candidate) )
		return candidate;

	return VK_NULL_HANDLE;
}


constexpr const char* needed_device_extensions[] = 
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

constexpr VkPhysicalDeviceFeatures needed_features = []()
{
	VkPhysicalDeviceFeatures _needed_features = {};
	//TODO: if an optional feature like anisotropy isn't available,
	//		don't ask for it and make sure sampler doesn't use it
	_needed_features.samplerAnisotropy = VK_TRUE;
	return _needed_features;
}();


darray<VkQueueFamilyProperties> Get_Queue_Families
(
	VkPhysicalDevice physical_device
){
	uint32_t num_queue_families = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(
		physical_device,
		&num_queue_families,
		nullptr
	);

	darray<VkQueueFamilyProperties> queue_families(num_queue_families);
	vkGetPhysicalDeviceQueueFamilyProperties(
		physical_device,
		&num_queue_families,
		&queue_families[0]
	);
	return queue_families;
}


uint32_t Graphics_Engine::Select_Graphics_Queue_Family ()
{
	auto queue_families = Get_Queue_Families (vulkan_physical_device);

	for(uint32_t index = 0; index < queue_families.length(); ++index)
	{
		if(queue_families[index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			return index;
	}

	return 0;
}

uint32_t Graphics_Engine::Select_Presentation_Queue_Family ()
{
	auto queue_families = Get_Queue_Families (vulkan_physical_device);

	for(uint32_t index = 0; index < queue_families.length(); ++index)
	{
		VkBool32 has_presentation_support = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR (
			vulkan_physical_device,
			index,
			window_surface,
			&has_presentation_support
		);
		if(has_presentation_support)
			return index;
	}
	return 0;
}


//major todo - uniques algorithm
darray<uint32_t> Unique_Queue_Families
(
	uint32_t graphics_family,
	uint32_t presentation_family
){
	size_t num_unique_families = 1;
	if(graphics_family != presentation_family) num_unique_families = 2;

	darray<uint32_t> uniques(num_unique_families);

	uniques[0] = graphics_family;
	if(num_unique_families == 2) uniques[1] = presentation_family;
	
	return uniques;
}


darray<VkDeviceQueueCreateInfo> Get_QueueCreateInfos
(
	uint32_t graphics_family,
	uint32_t presentation_family
){
	//Queues can share - only create new for unique queue family
	auto unique_families = Unique_Queue_Families(
		graphics_family,
		presentation_family
	);

	darray<VkDeviceQueueCreateInfo> queue_infos(unique_families.length());
	constexpr float queue_priority = 1;

	VkDeviceQueueCreateInfo queue_template = {};
    queue_template.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_template.pNext 			= nullptr;
    queue_template.flags 			= 0x0;
    queue_template.queueFamilyIndex = 0xFFFFFFFF;//this gets set in loop
    queue_template.queueCount 		= 1;
    queue_template.pQueuePriorities = &queue_priority;

    for(size_t i = 0; i < queue_infos.length(); ++i)
    {
		queue_template.queueFamilyIndex = i;
		queue_infos[i] = queue_template;
	}

	return queue_infos;
}


VkResult Graphics_Engine::Create_Vulkan_Device ()
{
	//Choose queue families
	uint32_t graphics_family 	 = Select_Graphics_Queue_Family();
	uint32_t presentation_family = Select_Presentation_Queue_Family();

	//{graphics_queue, index}
	auto queue_infos = Get_QueueCreateInfos(
		graphics_family,
		presentation_family
	);

	VkDeviceCreateInfo device_info = {};
	device_info.sType =	VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext					=	nullptr;
	device_info.flags					=	0x0;
	device_info.queueCreateInfoCount	=	queue_infos.length();
	device_info.pQueueCreateInfos		=	&queue_infos[0];
	//device_info.enabledLayerCount		=	//IGNORED + DEPRACATED;
	//device_info.ppEnabledLayerNames	=	//IGNORED + DEPRACATED;
	device_info.enabledExtensionCount	=	1;
	device_info.ppEnabledExtensionNames	=	needed_device_extensions;
	device_info.pEnabledFeatures		=	&needed_features;

	auto r = vkCreateDevice(
		vulkan_physical_device,
		&device_info,
		nullptr,
		&vulkan_device
	);
	if(r != VK_SUCCESS) return r;

	vkGetDeviceQueue(
		vulkan_device,
		graphics_family,
		0,
		&this->graphics_queue
	);
	vkGetDeviceQueue(
		vulkan_device,
		presentation_family,
		0,
		&this->presentation_queue
	);
	return VK_SUCCESS;
}

darray<VkSurfaceFormatKHR> Get_Surface_Formats
(
	VkPhysicalDevice physical_device,
	VkSurfaceKHR 	 surface
){
	uint32_t num_formats = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		physical_device,
		surface,
		&num_formats,
		nullptr
	);
	darray<VkSurfaceFormatKHR> formats(num_formats);
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		physical_device,
		surface,
		&num_formats,
		&formats[0]
	);
	return formats;
}

VkSurfaceFormatKHR Graphics_Engine::Choose_Swapchain_Surface_Format ()
{
	constexpr VkSurfaceFormatKHR best_format =
	{
		VK_FORMAT_B8G8R8A8_UNORM,
		VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
	};

	auto surface_formats = Get_Surface_Formats(
		vulkan_physical_device,
		window_surface
	);
	//this is how the spec says we can use any format
	if(	surface_formats.length() == 1 
	and surface_formats[0].format == VK_FORMAT_UNDEFINED
	) return best_format;

	//otherwise see if best_format is in the list
	for(auto&& f : surface_formats)
	{
		if( f.format == best_format.format
		and f.colorSpace == best_format.colorSpace
		) return best_format;
	}

	//otherwise pick the least-worst format
	return surface_formats[0];
}

uint32_t Graphics_Engine::Choose_Swapchain_Image_Count ()
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		vulkan_physical_device,
		window_surface,
		&capabilities
	);
	uint32_t image_count = 3;
	image_count = max(image_count, capabilities.minImageCount);
	if(capabilities.maxImageCount > 0)//0 means unlimited max
		image_count = min(image_count, capabilities.maxImageCount);

	return image_count;
}

VkExtent2D Graphics_Engine::Choose_Swapchain_Extent ()
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		vulkan_physical_device,
		window_surface,
		&capabilities
	);

	if( capabilities.currentExtent.width  != 0xFFFFFFFF
	and capabilities.currentExtent.height != 0xFFFFFFFF
	) return capabilities.currentExtent;
	else
	{
		VkExtent2D r = capabilities.currentExtent;
		r.width  = min(r.width, capabilities.maxImageExtent.width);
		r.width  = max(r.width, capabilities.minImageExtent.width);
		r.height = min(r.height, capabilities.maxImageExtent.height);
		r.height = max(r.height, capabilities.minImageExtent.height);
		return r;
	}
}

darray<VkPresentModeKHR> Get_Swapchain_Present_Modes
(
	VkPhysicalDevice physical_device,
	VkSurfaceKHR 	 surface
){
	uint32_t num_present_modes = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		physical_device,
		surface,
		&num_present_modes,
		nullptr
	);
	darray<VkPresentModeKHR> present_modes(num_present_modes);
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		physical_device,
		surface,
		&num_present_modes,
		&present_modes[0]
	);
	return present_modes;
}

VkPresentModeKHR Graphics_Engine::Choose_Swapchain_Present_Mode ()
{
	auto available_modes = Get_Swapchain_Present_Modes(
		this->vulkan_physical_device,
		this->window_surface
	);
	for(auto&& m : available_modes)
	{
		if(m == VK_PRESENT_MODE_MAILBOX_KHR) return m;
	}
	return available_modes[0];
}

VkResult Graphics_Engine::Create_Swapchain ()
{
	//queue family info
	uint32_t graphics_family 	 = Select_Graphics_Queue_Family();
	uint32_t presentation_family = Select_Presentation_Queue_Family();
	uint32_t queue_family_indices[] = {
		graphics_family,
		presentation_family
	};
	constexpr uint32_t num_queue_families =
		sizeof(queue_family_indices) / sizeof(*queue_family_indices);
	VkSharingMode sharing_mode = VK_SHARING_MODE_CONCURRENT;
	if(graphics_family == presentation_family)
		sharing_mode = VK_SHARING_MODE_EXCLUSIVE;

	const auto surface_format = Choose_Swapchain_Surface_Format();

	VkSwapchainCreateInfoKHR swap_info = {};
    swap_info.sType =VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swap_info.pNext 				= nullptr;
    swap_info.flags 				= 0x0;
    swap_info.surface				= window_surface;
    swap_info.minImageCount			= Choose_Swapchain_Image_Count();
    swap_info.imageFormat			= surface_format.format;
    swap_info.imageColorSpace		= surface_format.colorSpace;
    swap_info.imageExtent			= Choose_Swapchain_Extent();
    swap_info.imageArrayLayers		= 1;
    swap_info.imageUsage 		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swap_info.imageSharingMode		= sharing_mode;
    swap_info.queueFamilyIndexCount	= num_queue_families;
    swap_info.pQueueFamilyIndices	= queue_family_indices;
    swap_info.preTransform		= VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swap_info.compositeAlpha		= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swap_info.presentMode			= Choose_Swapchain_Present_Mode();
    swap_info.clipped				= VK_TRUE;
    swap_info.oldSwapchain			= VK_NULL_HANDLE;

    return vkCreateSwapchainKHR(
    	this->vulkan_device,
    	&swap_info,
    	nullptr,
    	&this->vulkan_swapchain
    );
}




//RenderPass Creation

constexpr VkFormat Choose_Depth_Format ()//TODO
{
	return VK_FORMAT_D32_SFLOAT;
}

VkAttachmentDescription 
Graphics_Engine::Get_Color_Attachment_Description()
{
	const auto surface_format = Choose_Swapchain_Surface_Format();

	VkAttachmentDescription color_attach = {};
	color_attach.flags = 0x0;
	//format must match the swapchain img format
	color_attach.format 	= surface_format.format;
    color_attach.samples 	= VK_SAMPLE_COUNT_1_BIT;
    color_attach.loadOp 	= VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attach.storeOp 	= VK_ATTACHMENT_STORE_OP_STORE;
    color_attach.initialLayout 	= VK_IMAGE_LAYOUT_UNDEFINED;
    color_attach.finalLayout 	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    return color_attach;
}

VkAttachmentDescription 
Graphics_Engine::Get_Depth_Attachment_Description()
{
	VkAttachmentDescription depth_attach = {};
	depth_attach.flags = 0x0;
	//todo - must match format
	depth_attach.format 	= Choose_Depth_Format();
    depth_attach.samples 	= VK_SAMPLE_COUNT_1_BIT;
    depth_attach.loadOp 	= VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attach.storeOp 	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attach.stencilLoadOp 	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attach.initialLayout 	= VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attach.finalLayout =
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    return depth_attach;
}
//TODO
constexpr size_t COLOR_ATTACHMENT_INDEX = 0;
constexpr size_t DEPTH_ATTACHMENT_INDEX = 1;

//expected attachments in the framebuffer that will get rendered to
carray<2,VkAttachmentDescription>
Graphics_Engine::Get_Attachment_Descriptions()
{
	carray<2,VkAttachmentDescription> descriptions;
	descriptions[0] = Get_Color_Attachment_Description();
	descriptions[1] = Get_Depth_Attachment_Description();
	return descriptions;
}

constexpr VkAttachmentReference colorAttachmentRef =
{
	0,//index from descriptions array
	VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
};

constexpr VkAttachmentReference depthAttachmentRef =
{
	1,//index from descriptions array
	VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
};

VkResult Graphics_Engine::Create_Render_Pass ()
{
	const auto attachments = Get_Attachment_Descriptions();

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint 		= VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount 	= 1;
	subpass.pColorAttachments 		= &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkRenderPassCreateInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.pNext 			= nullptr;
    rp_info.flags 			= 0x0;
    rp_info.attachmentCount = attachments.length();
    rp_info.pAttachments 	= &attachments[0];
    rp_info.subpassCount 	= 1;
    rp_info.pSubpasses 		= &subpass;
    rp_info.dependencyCount = 0;//todo - is there a dependency?
    rp_info.pDependencies 	= nullptr;

	return vkCreateRenderPass(
		this->vulkan_device,
		&rp_info,
		nullptr,
		&this->render_pass
	);
}


//GraphicsPipeline Creation

VkViewport Full_Viewport (VkExtent2D const &window_extent)
{
	VkViewport viewport = {};
	viewport.x 			= 0.0f;
	viewport.y 			= 0.0f;
	viewport.width 		= window_extent.width;
	viewport.height 	= window_extent.height;
	viewport.minDepth 	= 0.0f;
	viewport.maxDepth 	= 1.0f;

	return viewport;
}

VkRect2D Full_Scissor (VkExtent2D const &window_extent)
{
	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = window_extent;

	return scissor;
}

//TODO - move this out of this file
engine_library::Result<VkShaderModule, VkResult> Create_Shader_Module
(
	VkDevice device,
	size_t code_size,
	const uint32_t* pCode
){
	VkShaderModuleCreateInfo module_info = {};
	module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	module_info.pNext 	 = nullptr;
	module_info.flags 	 = 0x0;
	module_info.codeSize = code_size;
	module_info.pCode 	 = pCode;

	VkShaderModule shader_module = nullptr;
	VkResult result = vkCreateShaderModule(
		device,
		&module_info,
		nullptr,
		&shader_module
	);

	if(result != VK_SUCCESS) return result;
	else return {shader_module,result};
}

//TODO - put this in a different file. resource loader or something
#include <fstream>
darray<char> Slurp_File (const char* filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	
	if(!file.is_open())
	{
		std::cout << "Fatal: File doesn't exist - " << filename << std::endl;
		exit(420);
	}

	const auto file_size = file.tellg();
	darray<char> file_data(static_cast<size_t>(file_size));
	//slurp
	file.seekg(0);
	file.read(file_data.data(), file_size);
	file.close();

	return file_data;
}

//TODO - pass the code as engine_library::array<char> not filename
//TODO: this should return a Result<VkShaderModule[2]>
carray<2,VkShaderModule> Create_Shader_Modules
(
	const VkDevice device,
	const char* vert_shader_file,
	const char* frag_shader_file
){
	//vert shader
	auto vert_shader_code = Slurp_File(vert_shader_file);
	const auto&& vert_module_result = Create_Shader_Module(
		device,
		vert_shader_code.length(),
		reinterpret_cast<uint32_t*>(vert_shader_code.data())//TODO- possible alignment issue
	);
	VkShaderModule vert_module;
	if(vert_module_result.error) vert_module = VK_NULL_HANDLE;
	else vert_module = vert_module_result.value;

	//frag shader
	auto frag_shader_code = Slurp_File(frag_shader_file);
	const auto&& frag_module_result = Create_Shader_Module(
		device,
		frag_shader_code.length(),
		reinterpret_cast<uint32_t*>(frag_shader_code.data())//TODO- possible alignment issue
	);
	VkShaderModule frag_module;
	if(frag_module_result.error) frag_module = VK_NULL_HANDLE;
	else frag_module = frag_module_result.value;
	
	carray<2,VkShaderModule> shader_modules;
	shader_modules[0] = vert_module;
	shader_modules[1] = frag_module;
	return shader_modules;
}

void Destroy_Shader_Modules (VkDevice device, const carray<2,VkShaderModule> modules)
{
	for(auto&& m : modules)	vkDestroyShaderModule(device, m, nullptr);
}

carray<2,VkPipelineShaderStageCreateInfo> Graphics_Engine::Get_Shader_Stages
(
	const carray<2,VkShaderModule> shader_modules
){
	carray<2,VkPipelineShaderStageCreateInfo> shader_stages;
	VkPipelineShaderStageCreateInfo stage_template = {};
	stage_template.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stage_template.pNext = nullptr;
	stage_template.flags = 0x0;
	stage_template.pSpecializationInfo = nullptr;
	//vertex shader
	shader_stages[0] = stage_template;
	shader_stages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
	shader_stages[0].module = shader_modules[0];
	shader_stages[0].pName  = "main";
	//fragment shader
	shader_stages[1] = stage_template;
	shader_stages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
	shader_stages[1].module = shader_modules[1];
	shader_stages[1].pName  = "main";

	return shader_stages;
}

constexpr VkPipelineInputAssemblyStateCreateInfo input_assembly =
{
	VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
	nullptr,
	0x0,
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	VK_FALSE
};

//FF pipeline
constexpr VkPipelineRasterizationStateCreateInfo rasterization_state =
{
/*	sType	*/VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
/*	pNext					*/	nullptr,
/*	flags					*/	0x0,
/*	depthClampEnable		*/	VK_FALSE,
/*	rasterizerDiscardEnable	*/	VK_FALSE,
/*	polygonMode				*/	VK_POLYGON_MODE_FILL,
/*	cullMode				*/	VK_CULL_MODE_BACK_BIT,
/*	frontFace				*/	VK_FRONT_FACE_COUNTER_CLOCKWISE,
/*	depthBiasEnable			*/	VK_FALSE,
/*	depthBiasConstantFactor	*/	0.0f,
/*	depthBiasClamp			*/	0.0f,
/*	depthBiasSlopeFactor	*/	0.0f,
/*	lineWidth				*/	1.0f
};

constexpr VkPipelineMultisampleStateCreateInfo multisample_state = 
{
/*	sType	*/VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
/*	pNext					*/	nullptr,
/*	flags					*/	0x0,
/*	rasterizationSamples	*/	VK_SAMPLE_COUNT_1_BIT,
/*	sampleShadingEnable		*/	VK_FALSE,
/*	minSampleShading		*/	1.0f,
/*	pSampleMask				*/	nullptr,
/*	alphaToCoverageEnable	*/	VK_FALSE,
/*	alphaToOneEnable		*/	VK_FALSE
};

constexpr VkPipelineDepthStencilStateCreateInfo depth_stencil_state =
{
/*	sType	*/VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
/*	pNext 					*/nullptr,
/*	flags 					*/0x0,
/*	depthTestEnable 		*/VK_TRUE,
/*	depthWriteEnable 		*/VK_TRUE,
/*	depthCompareOp 			*/VK_COMPARE_OP_LESS_OR_EQUAL,
/*	depthBoundsTestEnable	*/VK_FALSE,
/*	stencilTestEnable		*/VK_FALSE,
/*	front 					*/{},
/*	back 					*/{},
/*	minDepthBounds 			*/0.0f,
/*	maxDepthBounds 			*/1.0f
};

constexpr VkPipelineColorBlendAttachmentState color_blend_attachment = 
{
/*	blendEnable			*/	VK_TRUE,
/*	srcColorBlendFactor	*/	VK_BLEND_FACTOR_SRC_ALPHA,
/*	dstColorBlendFactor	*/	VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
/*	colorBlendOp		*/	VK_BLEND_OP_ADD,
/*	srcAlphaBlendFactor	*/	VK_BLEND_FACTOR_ONE,
/*	dstAlphaBlendFactor	*/	VK_BLEND_FACTOR_ZERO,
/*	alphaBlendOp		*/	VK_BLEND_OP_ADD,
/*	colorWriteMask		*/	VK_COLOR_COMPONENT_R_BIT | 
							VK_COLOR_COMPONENT_G_BIT | 
							VK_COLOR_COMPONENT_B_BIT | 
							VK_COLOR_COMPONENT_A_BIT
};

constexpr VkPipelineColorBlendStateCreateInfo color_blend_state = 
{
/*	sType 	*/VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
/*	pNext				*/	nullptr,
/*	flags				*/	0x0,
/*	logicOpEnable		*/	VK_FALSE,
/*	logicOp				*/	VK_LOGIC_OP_COPY,
/*	attachmentCount		*/	1,
/*	pAttachments		*/	&color_blend_attachment,
/*	blendConstants[4]	*/	{}
};


VkResult Graphics_Engine::Create_Graphics_Pipeline ()
{
	const auto shader_modules = Create_Shader_Modules(
		this->vulkan_device,
		vert_shader_file,
		frag_shader_file
	);
	const auto shader_stages = Get_Shader_Stages(shader_modules);
	//TODO - carry extent over from earlier
	const auto window_extent = Choose_Swapchain_Extent();
	const auto viewport = Full_Viewport	(window_extent);
	const auto scissor 	= Full_Scissor	(window_extent);

	VkPipelineViewportStateCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	view_info.pNext 			= nullptr;
	view_info.flags 			= 0x0;
	view_info.viewportCount 	= 1;
	view_info.pViewports 		= &viewport;
	view_info.scissorCount 		= 1;
	view_info.pScissors 		= &scissor;

	VkGraphicsPipelineCreateInfo pipe_info = {};
	pipe_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipe_info.pNext 				= NULL;
	pipe_info.flags 				= 0x0;
	//shader program
	pipe_info.stageCount 			= shader_stages.length();
	pipe_info.pStages 				= shader_stages.data();
	pipe_info.pVertexInputState 	= &vertex_input_description;
	pipe_info.pInputAssemblyState 	= &input_assembly;
	pipe_info.pTessellationState 	= nullptr;	//no tesselation shader - ignored
	//viewport
	pipe_info.pViewportState 		= &view_info;
	//fixed function stages
	pipe_info.pRasterizationState 	= &rasterization_state;	//constexpr
	pipe_info.pMultisampleState 	= &multisample_state;	//constexpr
	pipe_info.pDepthStencilState 	= &depth_stencil_state;	//constexpr
	pipe_info.pColorBlendState 		= &color_blend_state;	//constexpr
	pipe_info.pDynamicState 		= nullptr;				//constexpr
	//resource layout
	pipe_info.layout 				= pipeline_layout;
	//in what render pass is this pipeline used
	pipe_info.renderPass 			= this->render_pass;
	pipe_info.subpass 				= 0;
	//parent pipeline. default is no parent
	pipe_info.basePipelineHandle 	= VK_NULL_HANDLE;
	pipe_info.basePipelineIndex 	= -1;
	auto result = vkCreateGraphicsPipelines(
		this->vulkan_device,
		VK_NULL_HANDLE_ARG<VkPipelineCache>,
		1,
		&pipe_info,
		nullptr,
		&this->graphics_pipeline
	);

	Destroy_Shader_Modules(this->vulkan_device, shader_modules);
	
	return result;
}

struct shader_binding
{
	//binding
	//type
	//array_length
	//shader_stage
};


VkResult Graphics_Engine::Create_Descriptor_Layouts ()
{
	//TODO: Create_Scene_Descriptor_Layout()
	//TODO: Create_EntityType_Descriptor_Layout()
	//TODO: Create_Entity_Descriptor_Layout()
	//todo - get this from shader_description.cxx
	//per_Scene
    VkDescriptorSetLayoutBinding scene_info = {};
	scene_info.binding 			= 0;//todo - shader_defines.h
	scene_info.descriptorType  	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	scene_info.descriptorCount 	= 1;
	scene_info.stageFlags 		= VK_SHADER_STAGE_VERTEX_BIT;
	//todo - get this from shader_description.cxx
    //per_EntityType
    VkDescriptorSetLayoutBinding entityType_info = {};
	entityType_info.binding 		= 0;//todo - shader_defines.h
	entityType_info.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	entityType_info.descriptorCount = 1;
	entityType_info.stageFlags 		= VK_SHADER_STAGE_FRAGMENT_BIT;
	//todo - get this(partially) from shader_description.cxx
	//per_Entity
    VkDescriptorSetLayoutBinding entity_info = {};
    entity_info.binding 		= 0;//todo - shader_defines.h
    entity_info.descriptorType 	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    entity_info.descriptorCount	= 1;
    entity_info.stageFlags 		= VK_SHADER_STAGE_VERTEX_BIT;

	//todo - shader_description can return these

	VkDescriptorSetLayoutCreateInfo scene_layout = {};
	scene_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	scene_layout.bindingCount = 1;
	scene_layout.pBindings    = &scene_info;

	VkDescriptorSetLayoutCreateInfo entityType_layout = {};
	entityType_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	entityType_layout.bindingCount = 1;
	entityType_layout.pBindings    = &entityType_info;

	VkDescriptorSetLayoutCreateInfo entity_layout = {};
	entity_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	entity_layout.bindingCount = 1;
	entity_layout.pBindings    = &entity_info;

	const auto&& scene_result = this->vulkan_device.CreateDescriptorSetLayout(
		&scene_layout
	);
	if(scene_result.error) return scene_result.error;
	constexpr auto scene_index = Descriptor_Set_Index::Scene;
	this->descriptor_set_layouts[scene_index] = scene_result.value;

	const auto&& entityType_result = this->vulkan_device.CreateDescriptorSetLayout(
		&entityType_layout
	);
	if(entityType_result.error) return entityType_result.error;
	constexpr auto entityType_index = Descriptor_Set_Index::EntityType;
	this->descriptor_set_layouts[entityType_index] = entityType_result.value;

	const auto&& entity_result = this->vulkan_device.CreateDescriptorSetLayout(
		&entity_layout
	);
	constexpr auto entity_index = Descriptor_Set_Index::Entity;
	this->descriptor_set_layouts[entity_index] = entity_result.value;
	return entity_result.error;
}

VkResult Graphics_Engine::Create_Pipeline_Layout ()
{
	VkPipelineLayoutCreateInfo pipe_layout_info = {};
	pipe_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipe_layout_info.pNext 					= nullptr;
	pipe_layout_info.flags 					= 0x0;
	pipe_layout_info.setLayoutCount 		= descriptor_set_layouts.length();
	pipe_layout_info.pSetLayouts 			= descriptor_set_layouts.data();
	pipe_layout_info.pushConstantRangeCount = 0;
	pipe_layout_info.pPushConstantRanges 	= nullptr;

	return vkCreatePipelineLayout(
		this->vulkan_device,
		&pipe_layout_info,
		nullptr,
		&this->pipeline_layout
	);
}


//todo - rendering environment is done. draw something.
//need to create actual resources for environment to use
//output resources - color and depth attachment
//input resources - vertex buffer, index buffer, texture sampler, uniform buffer
//command buffers

//Swapchain Images - used as color attachment in render pass
darray<VkImage> Get_Swapchain_Images
(
	const VkDevice device,
	const VkSwapchainKHR swapchain
){
	uint32_t image_count = 0;
	vkGetSwapchainImagesKHR(
		device,
		swapchain,
		&image_count,
		nullptr
	);
	darray<VkImage> swapchain_images(image_count);
	vkGetSwapchainImagesKHR(
		device,
		swapchain,
		&image_count,
		swapchain_images.data()
	);
	return swapchain_images;
}

VkResult Graphics_Engine::Create_Swapchain_Views ()
{
	const auto swapchain_images = Get_Swapchain_Images(vulkan_device,vulkan_swapchain);
	const auto surface_format = Choose_Swapchain_Surface_Format();
	//TODO: remember format during setup
	swapchain_image_views.resize(swapchain_images.length());

	const size_t length = swapchain_images.length();
	for(size_t i = 0; i < length; i++)
	{
		const auto view_info = Basic_ImageViewCreateInfo(
			swapchain_images[i], surface_format.format
		);
		const auto&& result = this->vulkan_device.CreateImageView(&view_info);
		this->swapchain_image_views[i] = result.value;
		if(result.error) return result.error;
	}	
	return VK_SUCCESS;
}

//TODO: memoryTypeBits is deterministic.
//pre-calculate where a buffer/image memory should go. Calculate_Memory_Indexes() or so
uint32_t Find_Memory_Index (
	VkPhysicalDevice	  physical_device,
	uint32_t			  memoryTypeBits,//Nth bit is set if Nth memoryType is ok
	VkMemoryPropertyFlags required_properties
){
	VkPhysicalDeviceMemoryProperties device_memory;
	vkGetPhysicalDeviceMemoryProperties(
		physical_device,
		&device_memory
	);
	for (uint32_t i = 0; i < device_memory.memoryTypeCount; i++)
	{
		const auto flags = device_memory.memoryTypes[i].propertyFlags; 
		if( memoryTypeBits & (1u << i) //Check Nth bit
			//Check if this memoryType meets all required_properties
		and required_properties == (flags & required_properties)
		) return i;
	}

	std::cout << "failed to find suitable memory type!\n";
	return device_memory.memoryTypeCount;//this should be invalid
}

//Depth Buffer - must match swapchain extent
VkResult Graphics_Engine::Create_Depth_Image ()
{
	const auto swap_extent = Choose_Swapchain_Extent();//TODO
	VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType 	 = VK_IMAGE_TYPE_2D;
    image_info.format 		 = Choose_Depth_Format();//TODO
    image_info.extent 		 = {swap_extent.width, swap_extent.height, 1};
    image_info.mipLevels 	 = 1;
    image_info.arrayLayers   = 1;
    image_info.samples 	  	 = VK_SAMPLE_COUNT_1_BIT;
    image_info.tiling 		 = VK_IMAGE_TILING_OPTIMAL;
    image_info.usage 		 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; 

	const auto&& result = this->vulkan_device.CreateImage(&image_info);
	this->depth_buffer.image = result.value;
	return result.error;
}

VkResult Graphics_Engine::Create_Depth_Memory ()
{
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(
		this->vulkan_device,
		this->depth_buffer.image,
		&memory_requirements
	);
	VkMemoryAllocateInfo memory_info = {};
	memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_info.allocationSize = memory_requirements.size;
	memory_info.memoryTypeIndex = Find_Memory_Index(
			this->vulkan_physical_device,
			memory_requirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
	const auto&& result = this->vulkan_device.AllocateMemory(&memory_info);
	this->depth_buffer.memory = result.value;
	return result.error;
}

VkResult Graphics_Engine::Create_Depth_View ()
{
	const auto view_info = Basic_ImageViewCreateInfo(
		this->depth_buffer.image, Choose_Depth_Format()
	);
	const auto&& result = this->vulkan_device.CreateImageView(&view_info);
	this->depth_buffer.view = result.value;
	return result.error;
}


VkResult Graphics_Engine::Transition_Depth_Layout ()//TODO: this might be optional
{
	return Execute_Commands_Now(
		this->vulkan_device,
		this->command_pool,
		this->graphics_queue,
		[depth_image = this->depth_buffer.image](VkCommandBuffer command_buffer)
	{
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.srcAccessMask = 0x0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
							  | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = depth_image;
		barrier.subresourceRange = Basic_Image_Range(VK_IMAGE_ASPECT_DEPTH_BIT);
		vkCmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			0x0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}
	);
}

VkResult Graphics_Engine::Create_Depth_Buffer ()
{
	VkFormat depth_buffer_format = Choose_Depth_Format();//TODO
	VkResult r;
	if((r = Create_Depth_Image ()) != VK_SUCCESS) return r;
	if((r = Create_Depth_Memory()) != VK_SUCCESS) return r;
	r = vkBindImageMemory(
		vulkan_device,
		depth_buffer.image,
		depth_buffer.memory,
		0
	);
	if(r != VK_SUCCESS) return r; 
	r = Create_Depth_View();
	if(r != VK_SUCCESS) return r;
	//TODO - transition might be optional?
	return Transition_Depth_Layout();
}

//Command Pool
VkResult Graphics_Engine::Create_Command_Pool ()//TODO - create transient pool too
{
	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = Select_Graphics_Queue_Family();//TODO

	const auto&& result = this->vulkan_device.CreateCommandPool(&pool_info);
	this->command_pool = result.value;
	return result.error;
}

//Framebuffer

VkResult Graphics_Engine::Create_Framebuffers()
{
	const auto num_framebuffers = swapchain_image_views.length();
	const auto swapchain_extent = Choose_Swapchain_Extent();
	this->framebuffers.resize(num_framebuffers);
	for(auto i = 0u; i < num_framebuffers; ++i)
	{
		engine_library::carray<2,VkImageView> attachments = {
			swapchain_image_views[i],
			depth_buffer.view
		};
		VkFramebufferCreateInfo fb_info = {};
		fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fb_info.renderPass = this->render_pass;
		fb_info.attachmentCount = 2;
		fb_info.pAttachments = &attachments[0];
		fb_info.width  = swapchain_extent.width;
		fb_info.height = swapchain_extent.height;
		fb_info.layers = 1;
		const auto&& result = this->vulkan_device.CreateFramebuffer(&fb_info);
		this->framebuffers[i] = result.value;
		if(result.error != VK_SUCCESS) return result.error;
	}
	return VK_SUCCESS;
}

//TODO: use overloaded functions? Vulkan_Copy(...)? Vulkan_Utility::Copy_Memory(...)?
//Copy_Data_To_Buffer(), Stage_Data_To_Buffer
//VkResult Copy_Buffer_To_Buffer(VkBuffer src, VkBuffer dst, size_t num_bytes);

//Rendering Resources
//VkResult Graphics_Engine::Register_Vertex_Buffer()
//VkResult Graphics_Engine::Register_Index_Buffer ()
//Resource Buffers

/*
using Vulkan_Utility::Result;
Result<VkBuffer> Graphics_Engine::Create_Buffer()
{
	VkBufferCreateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

	VkBuffer buffer;
	VkResult r = vkCreateBuffer(
		this->vulkan_device,
		&buffer_info,
		nullptr,
		&buffer
	);
	if(r != VK_SUCCESS) return r;
	else return buffer;
}
*/

Graphics_Engine::Result<VkDeviceMemory> Graphics_Engine::Alloc_Buffer_Memory
(
	const VkBuffer buffer,
	const VkMemoryPropertyFlags property_flags
){
	const auto requirements =
		this->vulkan_device.GetBufferMemoryRequirements(buffer);
	VkMemoryAllocateInfo memory_info = {};
	memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_info.allocationSize = requirements.size;
	memory_info.memoryTypeIndex = Find_Memory_Index(
		this->vulkan_physical_device,
		requirements.memoryTypeBits,
		property_flags
	);
	const auto&& result = this->vulkan_device.AllocateMemory(&memory_info);
	return result;
}

/*
 * Create Vertex Buffer:
 *  -Create Buffer Handle
 *  -Create Buffer Memory
 *  -Bind Buffer Memory
 *  -Create Staging Buffer handle, memory, bind
 *  -Copy data into staging buffer. Host -> Buffer
 *  -Copy staging buffer to vertex buffer. Buffer -> Buffer
 *  Done
 */

Graphics_Engine::Result<Mesh> Graphics_Engine::Register_Mesh(
	const Mesh_Data& mesh_data
){
	//TODO: simplify this function
	//TODO: <VkBuffer,VkMemory> Create_Vertex_Buffer(vertex_data);
	//TODO: <VkBuffer,VkMemory> Create_Index_Buffer (index_data);
	//TODO: Stage_Data_To_Buffer() - upload to device local buffer
	//TODO: Create_Buffer(size, usage, memory_properties, initial_data)
	Mesh mesh = {};
	{
		//Create Vertex Buffer
		VkBufferCreateInfo vertex_buffer_info = {};
		vertex_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertex_buffer_info.size = mesh_data.vertexes.size();
		vertex_buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		vertex_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		const auto&& buffer_result = 
			this->vulkan_device.CreateBuffer(&vertex_buffer_info);
		if(buffer_result.error) return {mesh,buffer_result.error};
		const auto vertex_buffer = buffer_result.value;
		mesh.draw_info.vertex_buffer = vertex_buffer;
		//Assign Memory
		const auto&& memory_result = Alloc_Buffer_Memory(
			vertex_buffer,
			  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT //TODO
			| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		if(memory_result.error) return {mesh,memory_result.error};
		const auto vertex_memory = memory_result.value;
		mesh.memory.vertex_memory = vertex_memory;
		const auto&& bind_result = this->vulkan_device.BindBufferMemory(
			vertex_buffer, vertex_memory, 0
		);
		if(bind_result != VK_SUCCESS) return bind_result;
	}{
		//Create Index Buffer
		VkBufferCreateInfo index_buffer_info = {};
		index_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		index_buffer_info.size = mesh_data.indexes.size();
		index_buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		index_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		const auto&& buffer_result =
			this->vulkan_device.CreateBuffer(&index_buffer_info);
		if(buffer_result.error) return {mesh,buffer_result.error};
		const auto index_buffer = buffer_result.value;
		mesh.draw_info.index_buffer = index_buffer;
		//Assign Memory
		const auto&& memory_result = Alloc_Buffer_Memory(
			index_buffer,
			  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT //TODO
			| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		if(memory_result.error) return {mesh,memory_result.error};
		const auto index_memory = memory_result.value;
		mesh.memory.index_memory = index_memory;
		const auto&& bind_result = this->vulkan_device.BindBufferMemory(
			index_buffer, index_memory, 0
		);
		if(bind_result != VK_SUCCESS) return bind_result;
	}
	//Remember how many indices
	mesh.draw_info.num_indexes = mesh_data.indexes.length();
	
	//TODO - temp mesh sucks
	//TODO - upload correct data. use staging buffer
	void* mem_ptr;
	vkMapMemory(
		this->vulkan_device,
		mesh.memory.index_memory,
		0,
		VK_WHOLE_SIZE,
		0x0,
		&mem_ptr
	);
	memcpy(mem_ptr, mesh_data.indexes.data(), mesh_data.indexes.size());
	vkUnmapMemory(
		this->vulkan_device,
		mesh.memory.index_memory
	);
	vkMapMemory(
		this->vulkan_device,
		mesh.memory.vertex_memory,
		0,
		VK_WHOLE_SIZE,
		0x0,
		&mem_ptr
	);
	memcpy(mem_ptr, mesh_data.vertexes.data(), mesh_data.vertexes.size());
	vkUnmapMemory(
		this->vulkan_device,
		mesh.memory.vertex_memory
	);
	return {mesh, VK_SUCCESS};
}

//TODO - put this statement somewhere else
template <typename T>
using Result = Graphics_Engine::Result<T>;

Result<VkDeviceMemory> Graphics_Engine::Alloc_Image_Memory
(
	const VkImage image,
	const VkMemoryPropertyFlags property_flags
){
	const auto requirements =
		this->vulkan_device.GetImageMemoryRequirements(image);
	VkMemoryAllocateInfo memory_info = {};
	memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_info.allocationSize = requirements.size;
	memory_info.memoryTypeIndex = Find_Memory_Index(
		this->vulkan_physical_device,
		requirements.memoryTypeBits,
		property_flags
	);
	const auto&& result = this->vulkan_device.AllocateMemory(&memory_info);
	return result;
}

/*
VkResult graphics_engine::Transition_Image_Layout
(
	VkImage image,
	VkImageAspectFlags aspectMask,
	VkImageLayout old_layout,
	VkImageLayout new_layout
){
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;//TODO
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	barrier.oldLayout = old_layout;
	barrier.newLayout = new_layout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange = Basic_Image_Range(aspectMask);
	vkCmdPipelineBarrier(
		command_buffer,
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0x0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
}*/

//TODO: Result<VkImage> Create_Basic_Image(device, width+height, format, usage);
//TODO: Result<VkImageView> Create_Basic_ImageView(device, image, format);
Result<Texture_Resources> Graphics_Engine::Register_Texture
(
	const Texture_Data &texture_data
){
	//TODO
	//make VkImage
	VkImageCreateInfo image_info = {};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.flags		 = 0x0;
	image_info.imageType	 = VK_IMAGE_TYPE_2D;
	image_info.format		 = texture_data.format;
	image_info.extent		 = {texture_data.width, texture_data.height, 1};
	image_info.mipLevels	 = 1;
	image_info.arrayLayers	 = 1;
	image_info.samples		 = VK_SAMPLE_COUNT_1_BIT;
	image_info.tiling		 = VK_IMAGE_TILING_LINEAR;//VK_IMAGE_TILING_OPTIMAL;//TODO
	image_info.usage		 = VK_IMAGE_USAGE_SAMPLED_BIT;
	image_info.sharingMode	 = VK_SHARING_MODE_EXCLUSIVE;
	image_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;//VK_IMAGE_LAYOUT_UNDEFINED;//TODO
	const auto&& image_result = this->vulkan_device.CreateImage(&image_info);
	if(image_result.error) return image_result.error;
	//alloc+bind vkDeviceMemory
	const auto&& memory_result = Alloc_Image_Memory(
		image_result.value,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	if(memory_result.error) return memory_result.error;
	const auto&& bind_result = vkBindImageMemory(
		this->vulkan_device, image_result.value, memory_result.value, 0
	);
	if(bind_result != VK_SUCCESS) return bind_result;
	//copy data into device memory (major TODO - should be staged into device memory)
	void* data_ptr;
	vkMapMemory(
		this->vulkan_device,
		memory_result.value,
		0,
		VK_WHOLE_SIZE,
		0x0,
		&data_ptr
	);
	memcpy(data_ptr, texture_data.data, texture_data.width*texture_data.height*4);
	vkUnmapMemory(this->vulkan_device, memory_result.value);

	//Transition image layout (MAJOR TODO?)
	VkImage texture_image = image_result.value;
	const auto&& todo_error = Execute_Commands_Now(
		this->vulkan_device,
		this->command_pool,
		this->graphics_queue,
		[texture_image](VkCommandBuffer command_buffer)
	{
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;//TODO
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;//TODO
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = texture_image;
		barrier.subresourceRange = Basic_Image_Range(VK_IMAGE_ASPECT_COLOR_BIT);
		vkCmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_HOST_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0x0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}
	);
	if(todo_error) return todo_error;

	//make vkImageView
	const auto&& view_info = Basic_ImageViewCreateInfo(image_result.value, texture_data.format);
	const auto&& view_result = this->vulkan_device.CreateImageView(&view_info);
	if(view_result.error) return view_result.error;
	//allocate descriptor
	const auto&& descriptor_result = Create_Texture_Descriptor(view_result.value);
	//Return created things
	return {{
		image_result.value,
		view_result.value,
		memory_result.value,
		descriptor_result.value
	}, VK_SUCCESS};
}

//Can use one sampler in many CombinedImageSampler descriptor sets
VkResult Graphics_Engine::Create_Texture_Sampler()
{
	VkSamplerCreateInfo sampler_info = {};
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter 		  = VK_FILTER_LINEAR;
    sampler_info.minFilter 		  = VK_FILTER_LINEAR;
    sampler_info.mipmapMode 	  = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.addressModeU 	  = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV 	  = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW 	  = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.mipLodBias 	  = 0.0f;
    sampler_info.anisotropyEnable = VK_TRUE;//TODO: conditional on availability
    sampler_info.maxAnisotropy 	  = 16.0f;//TODO: conditional on availability
    sampler_info.compareEnable 	  = VK_FALSE;
    sampler_info.borderColor 	  = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
	const auto&& result = this->vulkan_device.CreateSampler(&sampler_info);
	this->texture_sampler = result.value;
	return result.error;
}

//Per-Object Dynamic Uniform Buffer
VkDeviceSize Graphics_Engine::Get_Uniform_Alignment(size_t uniform_size)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(this->vulkan_physical_device, &properties);
	const auto minimum_alignment =
		properties.limits.minUniformBufferOffsetAlignment;
	if(uniform_size <= minimum_alignment) return minimum_alignment;
	if(uniform_size % minimum_alignment == 0) return uniform_size;
	else return minimum_alignment * (uniform_size / minimum_alignment + 1);
}

//TODO
VkResult Graphics_Engine::Init_Entity_Uniform()
{
	const auto&& aligned_size = Get_Uniform_Alignment(sizeof(Entity_Uniform));
	const auto&& buffer_result = Create_Buffer_Handle(
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, aligned_size*max_entity_per_draw
	);
	if(buffer_result.error) return buffer_result.error;
	this->per_entity_uniform_buffer = buffer_result.value;
	const auto&& memory_result = Alloc_Buffer_Memory(
		this->per_entity_uniform_buffer,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	if(memory_result.error) return memory_result.error;
	this->per_entity_uniform_memory = memory_result.value;
	const auto&& bind_result = this->vulkan_device.BindBufferMemory(
		per_entity_uniform_buffer, per_entity_uniform_memory, 0
	);
	if(bind_result != VK_SUCCESS) return bind_result;
	const auto&& map_result = Map_Whole_Memory(per_entity_uniform_memory);
	if(map_result.error) return map_result.error;
	this->per_entity_uniform_ptr = map_result.value;
	const mat4 identity = mat4::identity();
	for(auto i = 0u; i < max_entity_per_draw; ++i)
	{
		memcpy(
			static_cast<Entity_Uniform*>(per_entity_uniform_ptr) + i,
			&identity,
			sizeof(identity)
		);
	}
	return VK_SUCCESS;
}

Result<VkBuffer> Graphics_Engine::Create_Buffer_Handle
(
	const VkBufferUsageFlagBits usage,
	const VkDeviceSize			size
){
	VkBufferCreateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size 		= size;
	buffer_info.usage 		= usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	const auto&& result = this->vulkan_device.CreateBuffer(&buffer_info);
	return result;
}

Result<void*> Graphics_Engine::Map_Whole_Memory(VkDeviceMemory memory)
{
	void* mapped_ptr;
	const auto&& result = vkMapMemory(
		this->vulkan_device,
		memory,
		0,				//offset
		VK_WHOLE_SIZE,	//size
		0x0,			//flags
		&mapped_ptr
	);
	return {mapped_ptr, result};
}

void Graphics_Engine::Unmap_Memory(VkDeviceMemory memory)
{
	return vkUnmapMemory(this->vulkan_device, memory);
}

VkResult Graphics_Engine::Init_Scene_Uniform()
{
	const auto&& buffer_result = Create_Buffer_Handle(
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(Camera)
	);
	if(buffer_result.error) return buffer_result.error;
	this->scene_uniform_buffer = buffer_result.value;
	const auto&& memory_result = Alloc_Buffer_Memory(
		this->scene_uniform_buffer,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	if(memory_result.error) return memory_result.error;
	this->scene_uniform_memory = memory_result.value;
	const auto&& bind_result = this->vulkan_device.BindBufferMemory(
		scene_uniform_buffer, scene_uniform_memory, 0
	);
	if(bind_result != VK_SUCCESS) return bind_result;
	const auto&& map_result = Map_Whole_Memory(scene_uniform_memory);
	if(map_result.error) return map_result.error;
	this->scene_uniform_ptr = map_result.value;
	Camera c;
	c.Make_Identity();
	memcpy(scene_uniform_ptr, &c, sizeof(c));
	return VK_SUCCESS;
}

//Resource Descriptors
VkResult Graphics_Engine::Create_Descriptor_Pool()
{
	//TODO - must should match # of resource types used in shader
	constexpr engine_library::carray<3,VkDescriptorPoolSize> pool_sizes = {{
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER		  , 1},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1},
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, this->max_textures}
	}};
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	//1 for camera uniform, 1 for per-entity dynamic uniform, 1 per unique texture
	constexpr uint32_t max_descriptor_sets = (1 + 1 + max_textures);
	pool_info.maxSets 		= max_descriptor_sets;
	pool_info.poolSizeCount = pool_sizes.length();
	pool_info.pPoolSizes 	= pool_sizes.data();

	const auto&& result = this->vulkan_device.CreateDescriptorPool(&pool_info);
	this->descriptor_pool = result.value;
	return result.error;
}

//TODO: do this
//Create_Scene_Descriptor
//Create_EntityType_Descriptors
//Create_Entity_Descriptor
VkResult Graphics_Engine::Create_Entity_Descriptor()
{
	//TODO: do this properly
	//Alloc descriptor sets
	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool 	  = this->descriptor_pool;
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts =
		&this->descriptor_set_layouts[Descriptor_Set_Index::Entity];
	const auto&& r = vkAllocateDescriptorSets(//TODO
		this->vulkan_device, &alloc_info, &this->entity_descriptor
	);
	if(r != VK_SUCCESS) return r;
	//TODO: allocate and update combined image samplers
	VkDescriptorBufferInfo buffer_info = {};
	buffer_info.buffer = this->per_entity_uniform_buffer;
	buffer_info.offset = 0;
	buffer_info.range  = VK_WHOLE_SIZE;
	VkWriteDescriptorSet buffer_descriptor = {};
	buffer_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	buffer_descriptor.dstSet = this->entity_descriptor;
	buffer_descriptor.dstBinding = 0;//TODO - shader_defines.h
	buffer_descriptor.descriptorCount = 1;
	buffer_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;//TODO: MUST match the VkDescriptorSetLayoutBinding specified in Create_Descriptor_Layouts()
	buffer_descriptor.pBufferInfo = &buffer_info;
	//TODO: batch update?
	vkUpdateDescriptorSets(
		this->vulkan_device, 1, &buffer_descriptor, 0, nullptr
	);
	return VK_SUCCESS;//vkUpdateDescriptorSets returns void
}

VkResult Graphics_Engine::Create_Scene_Descriptor()
{
	//TODO: do this properly
	//Alloc descriptor sets
	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool 	  = this->descriptor_pool;
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts =
		&this->descriptor_set_layouts[Descriptor_Set_Index::Scene];
	const auto&& r = vkAllocateDescriptorSets(//TODO
		this->vulkan_device, &alloc_info, &this->scene_descriptor
	);
	if(r != VK_SUCCESS) return r;
	//TODO: allocate and update combined image samplers
	VkDescriptorBufferInfo buffer_info = {};
	buffer_info.buffer = this->scene_uniform_buffer;
	buffer_info.offset = 0;
	buffer_info.range  = VK_WHOLE_SIZE;
	VkWriteDescriptorSet buffer_descriptor = {};
	buffer_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	buffer_descriptor.dstSet = this->scene_descriptor;
	buffer_descriptor.dstBinding = 0;//TODO - shader_defines.h
	buffer_descriptor.descriptorCount = 1;
	buffer_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//TODO: MUST match the VkDescriptorSetLayoutBinding specified in Create_Descriptor_Layouts()
	buffer_descriptor.pBufferInfo = &buffer_info;
	//TODO: batch update?
	vkUpdateDescriptorSets(
		this->vulkan_device, 1, &buffer_descriptor, 0, nullptr
	);
	return VK_SUCCESS;//vkUpdateDescriptorSets returns void
}

Result<VkDescriptorSet> Graphics_Engine::Create_Texture_Descriptor
(
	VkImageView texture_view
){
	//TODO: do this properly
	//Alloc descriptor sets
	VkDescriptorSet texture_descriptor;
	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool 	  = this->descriptor_pool;
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts =
		&this->descriptor_set_layouts[Descriptor_Set_Index::EntityType];
	const auto&& r = vkAllocateDescriptorSets(//TODO
		this->vulkan_device, &alloc_info, &texture_descriptor
	);
	if(r != VK_SUCCESS) return r;
	//TODO: allocate and update combined image samplers
	VkDescriptorImageInfo image_info = {};
	image_info.sampler = this->texture_sampler;
	image_info.imageView = texture_view;
	image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	VkWriteDescriptorSet descriptor_info = {};
	descriptor_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_info.dstSet = texture_descriptor;
	descriptor_info.dstBinding = 0;//TODO - shader_defines.h
	descriptor_info.descriptorCount = 1;
	descriptor_info.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_info.pImageInfo = &image_info;

	//allocate and update - specify buffer and texture
	//TODO: batch update?
	vkUpdateDescriptorSets(
		this->vulkan_device, 1, &descriptor_info, 0, nullptr
	);
	return {texture_descriptor, VK_SUCCESS};
}

//VkResult Graphics_Engine::Create_Descriptor_Sets()
//{
//	const auto&& entity_result = Create_Entity_Descriptor();//TODO
//	if(entity_result != VK_SUCCESS) return entity_result;	//TODO
//	const auto&& scene_result = Create_Scene_Descriptor();
//	return scene_result;
//}

//Semaphores
VkResult Graphics_Engine::Create_Semaphores()
{
	VkSemaphoreCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VkResult r = vkCreateSemaphore(
		this->vulkan_device,
		&info,
		nullptr,
		&this->image_available
	);
	if(r != VK_SUCCESS) return r;
	return vkCreateSemaphore(
		this->vulkan_device,
		&info,
		nullptr,
		&this->rendering_finished
	);
}


//Draw Buffer
VkResult Graphics_Engine::Allocate_Draw_Buffer()
{
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool 		  = this->command_pool;
	alloc_info.level 			  = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = 1;
	return vkAllocateCommandBuffers(
		this->vulkan_device,
		&alloc_info,
		&this->draw_buffer
	);
}

constexpr engine_library::carray<2,VkClearValue> Get_Clear_Values()
{
	VkClearValue color_clear = {};
	color_clear.color = {0.0f, 0.0f, 1.0f, 1.0f};

	VkClearValue depth_clear = {};
	depth_clear.depthStencil = {1.0f, 0};

	return {color_clear, depth_clear};
}

using namespace Graphics;
void Graphics_Engine::InsertCmd_Draw_Skybox(
	VkCommandBuffer command_buffer,
	Mesh_Draw_Info  skybox_mesh,//TODO
	VkDescriptorSet skybox_texture_descriptor
){
	//draw skybox
	InsertCmd_Bind_Mesh	  (command_buffer, skybox_mesh);
	InsertCmd_Bind_Texture(command_buffer, skybox_texture_descriptor);
	vkCmdBindDescriptorSets(
		command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		this->pipeline_layout,
		Descriptor_Set_Index::Entity,
		1, &this->entity_descriptor,
		1, &Graphics_Engine::skybox_offset
	);
	vkCmdDrawIndexed	  (command_buffer, skybox_mesh.num_indexes, 1, 0, 0, 0);
}

void Graphics_Engine::InsertCmd_Bind_Mesh
(
	VkCommandBuffer		 command_buffer,
	const Mesh_Draw_Info mesh
){
	vkCmdBindIndexBuffer(command_buffer, mesh.index_buffer, 0, INDEX_INT_TYPE);
	const VkDeviceSize vertex_offset = 0;
	vkCmdBindVertexBuffers(
		command_buffer, 0, 1, &mesh.vertex_buffer, &vertex_offset
	);
}


void Graphics_Engine::InsertCmd_Bind_Texture
(
	VkCommandBuffer 	  command_buffer,
	const VkDescriptorSet texture_descriptor
){
	vkCmdBindDescriptorSets(
		command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		this->pipeline_layout,
		static_cast<uint32_t>(Descriptor_Set_Index::EntityType),
		1, &texture_descriptor,
		0, nullptr
	);
}


void Graphics_Engine::InsertCmd_Bind_EntityType
(
	VkCommandBuffer 	  command_buffer,
	const EntityType_Info &entity_type
){
	InsertCmd_Bind_Mesh	  (command_buffer, entity_type.mesh);
	InsertCmd_Bind_Texture(command_buffer, entity_type.texture_descriptor);
}


void Graphics_Engine::InsertCmd_Draw_Entity_List(
	VkCommandBuffer   command_buffer,
	const Entity_List &entity_list
){
	InsertCmd_Bind_EntityType(command_buffer, entity_list.entity_type);
	for(auto&& entity : entity_list.entities)
	{
		vkCmdBindDescriptorSets(
			command_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			this->pipeline_layout,
			static_cast<uint32_t>(Descriptor_Set_Index::Entity),
			1, &entity_descriptor,
			1, &entity.offset//if each entity gets its own matrix then just calculate this to stay local. i * sizeof(Entity_Uniform)
		);
		vkCmdDrawIndexed(
			command_buffer,
			entity_list.entity_type.mesh.num_indexes,
			1, 0, 0, 0
		);
	}
}


void Graphics_Engine::InsertCmd_Begin_Frame
(
	VkCommandBuffer 	command_buffer,
	const VkFramebuffer target_framebuffer
){
	vkCmdBindPipeline(
		command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphics_pipeline
	);
	VkRenderPassBeginInfo rp_begin_info = {};
	rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rp_begin_info.renderPass = this->render_pass;
	rp_begin_info.framebuffer = target_framebuffer;
	rp_begin_info.renderArea.extent = Choose_Swapchain_Extent();//TODO
	const auto clear_values 	= Get_Clear_Values();
	rp_begin_info.clearValueCount 	= clear_values.length();
	rp_begin_info.pClearValues 		= clear_values.data();
	vkCmdBeginRenderPass(//TODO: use VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS?
		command_buffer, &rp_begin_info, VK_SUBPASS_CONTENTS_INLINE
	);
}


void Graphics_Engine::InsertCmd_Clear_Depth_Buffer
(
	VkCommandBuffer command_buffer
){
	VkClearAttachment clear_info = {};
	clear_info.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;//TODO - must match depth buffer
	clear_info.clearValue.depthStencil = {1.0, 0};
	VkClearRect clear_rect = {};
	clear_rect.rect.extent = Choose_Swapchain_Extent();//TODO
	clear_rect.baseArrayLayer = 0;
	clear_rect.layerCount = 1;
	vkCmdClearAttachments(command_buffer, 1, &clear_info, 1, &clear_rect);
}


void Graphics_Engine::InsertCmd_Draw_Scene(
	VkCommandBuffer 	command_buffer,
	const Scene 		&scene
){
	//bind scene descriptor
	vkCmdBindDescriptorSets(
		command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		this->pipeline_layout,
		Descriptor_Set_Index::Scene,
		1, &scene_descriptor,
		0, nullptr
	);
	//draw skybox
	InsertCmd_Draw_Skybox(command_buffer, scene.skybox_mesh, scene.skybox_texture_descriptor);
	//wipe depth buffer
	//TODO: does this need to be synchronized? probably.
	InsertCmd_Clear_Depth_Buffer(command_buffer);
	//draw everything else
	for(auto&& entity_list : scene.entity_lists)
	{
		InsertCmd_Draw_Entity_List(command_buffer, entity_list);
	}
}


VkResult Graphics_Engine::Generate_Draw_Commands(
	VkCommandBuffer 	command_buffer,
	const VkFramebuffer	target_framebuffer
){
	//TODO: do this
	//start command recording
	VkCommandBufferBeginInfo command_begin_info = {};
	command_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	VkResult r = vkBeginCommandBuffer(command_buffer, &command_begin_info);
	if(r != VK_SUCCESS) return r;
	//start render pass
	InsertCmd_Begin_Frame(command_buffer, target_framebuffer);
	//draw scene
	InsertCmd_Draw_Scene(command_buffer, temp.main_scene);
	//end render pass
	vkCmdEndRenderPass(command_buffer);
	return vkEndCommandBuffer(command_buffer);
}

//public-facing funcs
//init/cleanup
App_Error Graphics_Engine::init (const Graphics_Init_Info& init_info)
{
	//Setup basic vulkan environment
	if(auto e = Create_Vulkan_Instance	()) return {VkResult_to_CString(e)};
	if(auto e = Setup_Debug_Callback	()) return {VkResult_to_CString(e)};
	//Choose physical device
	this->vulkan_physical_device = Pick_Physical_Device();
	if(this->vulkan_physical_device == VK_NULL_HANDLE)
							return {"Unable to choose a physical device!"};
	//Register OS window with Vulkan
	if(auto e = Create_Window_Surface(init_info.window_handle))
												return {VkResult_to_CString(e)};
	//Create logical device
	if(auto e = Create_Vulkan_Device		()) return {VkResult_to_CString(e)};
	if(auto e = Create_Command_Pool			()) return {VkResult_to_CString(e)};
	//Setup rendering environment
	if(auto e = Create_Swapchain			())	return {VkResult_to_CString(e)};
	if(auto e = Create_Render_Pass			())	return {VkResult_to_CString(e)};
	if(auto e = Create_Descriptor_Layouts	()) return {VkResult_to_CString(e)};
	if(auto e = Create_Pipeline_Layout		()) return {VkResult_to_CString(e)};
	if(auto e = Create_Graphics_Pipeline	()) return {VkResult_to_CString(e)};
	if(auto e = Create_Swapchain_Views		()) return {VkResult_to_CString(e)};
	if(auto e = Create_Depth_Buffer			()) return {VkResult_to_CString(e)};
	if(auto e = Create_Framebuffers			()) return {VkResult_to_CString(e)};
	if(auto e = Allocate_Draw_Buffer		()) return {VkResult_to_CString(e)};
	if(auto e = Create_Semaphores			()) return {VkResult_to_CString(e)};
	if(auto e = Create_Texture_Sampler		()) return {VkResult_to_CString(e)};
	//Shader Uniforms
	if(auto e = Init_Scene_Uniform			()) return {VkResult_to_CString(e)};
	if(auto e = Init_Entity_Uniform			()) return {VkResult_to_CString(e)};
	//Descriptor Sets
	if(auto e = Create_Descriptor_Pool		()) return {VkResult_to_CString(e)};
	if(auto e = Create_Entity_Descriptor	()) return {VkResult_to_CString(e)};
	if(auto e = Create_Scene_Descriptor 	()) return {VkResult_to_CString(e)};


	return {nullptr};
}

void Graphics_Engine::cleanup()
{
	/*
	DestroyDebugReportCallbackEXT(
		this->vulkan_instance,
		this->debug_callback,
		nullptr
	);
	*/
	Unmap_Memory(this->scene_uniform_memory);
	Unmap_Memory(this->per_entity_uniform_memory);
	vkDestroyInstance(this->vulkan_instance, nullptr);
}

//drawing
App_Error Graphics_Engine::Draw_Frame()
{
	//TODO: multiple command buffers?
	//TODO - synchronization
	//get framebuffer index
	uint32_t framebuffer_index;
	vkAcquireNextImageKHR(
		this->vulkan_device,
		vulkan_swapchain,
		0xFFFFFFFF,
		image_available,
		VK_NULL_HANDLE_ARG<VkFence>,
		&framebuffer_index
	);
	//generate commands to draw to framebuffer
	Generate_Draw_Commands(draw_buffer, framebuffers[framebuffer_index]);
	//submit commands
	VkPipelineStageFlags wait_stage = //VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &this->image_available;
	submit_info.pWaitDstStageMask = &wait_stage;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &this->draw_buffer;
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &this->rendering_finished;
	vkQueueSubmit(
		graphics_queue,
		1,
		&submit_info,
		VK_NULL_HANDLE_ARG<VkFence>
	);
	VkResult presentation_result;
	//display framebuffer
	VkPresentInfoKHR presentation_info = {};
	presentation_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentation_info.waitSemaphoreCount = 1;
	presentation_info.pWaitSemaphores = &this->rendering_finished;
	presentation_info.swapchainCount = 1;
	presentation_info.pSwapchains = &this->vulkan_swapchain;
	presentation_info.pImageIndices = &framebuffer_index;
	presentation_info.pResults = &presentation_result;
	VkResult r = vkQueuePresentKHR(presentation_queue, &presentation_info);
	vkQueueWaitIdle(presentation_queue);
	if(r != VK_SUCCESS) return {VkResult_to_CString(r)};
	if(presentation_result != VK_SUCCESS) return {VkResult_to_CString(presentation_result)};
	else return {nullptr};
}
