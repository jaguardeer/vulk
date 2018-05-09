#ifndef GRAPHICS_ENGINE
#define GRAPHICS_ENGINE
extern "C" {
#include <vulkan/vulkan.h>
}
#include "vulkan_device.hxx"
#include "app_error.hxx"//TODO
#include "array.hxx"
#include "renderer.hxx"
#include "data_structs.hxx"
#include "result.hxx"
#include "shader_defines.h"

//TODO: direct initialize stuff
//TODO: implement span
namespace Graphics{

struct Mesh_Draw_Info
{
	VkBuffer vertex_buffer;
	VkBuffer index_buffer ;
	uint32_t num_indexes  ;
};

struct Mesh_Memory
{
	VkDeviceMemory vertex_memory;
	VkDeviceMemory index_memory;
};

struct Mesh
{
	Mesh_Draw_Info draw_info;
	Mesh_Memory	   memory;
};

struct Texture_Resources
{
	VkImage			image;
	VkImageView		view;
	VkDeviceMemory	memory;
	VkDescriptorSet descriptor;
};

constexpr VkIndexType INDEX_INT_TYPE = VK_INDEX_TYPE_UINT16;

class Camera
{
public:
	void Rotate		(const vec3 rotation);//[axis,angle] = [normal,length]
	void Translate	(const vec3 translation);
	void Set_FOV	(const float FOV_X_Radians);
	void Make_Identity()
	{
		matrix.view = mat4::identity();
		matrix.proj = mat4::identity();
	}
//private://TODO
	Camera_Matrix matrix;
};

struct EntityType_Info
{
	Mesh_Draw_Info  mesh;
	VkDescriptorSet texture_descriptor;
	//VkDescriptorSet model_matrix_descriptor;
};

struct Entity_Info
{
	uint32_t offset;//TODO: should I even do this?
};

struct Entity_List
{
	EntityType_Info entity_type;
	engine_library::darray<Entity_Info> entities;
};

struct Scene
{
	Camera camera;
	Mesh_Draw_Info  skybox_mesh;//TODO
	VkDescriptorSet skybox_texture_descriptor;
	engine_library::darray<Entity_List> entity_lists;//TODO
};

struct Graphics_Init_Info
{
	class Window_Handle;
	Window_Handle* window_handle;
	//uint32_t max_textures;//or make descriptor pool dynamic?
};




/*
TODO: have a setter-upper class?
TODO: finish per-entity uniform and make a test scene in test_graphics_engine.cxx
	  then compose a scene and draw it
TODO: test instanced rendering w/ uniform array vs dynamic offset
TODO: decide on containers for meshes+textures
TODO: make App_Error an engine_library::Result
MAJOR TODO: Split Vulkan_Environment from Renderer from Graphics_Engine
TODO: see if default vertex attribute can be specified
*/

class Graphics_Engine
{
public:
	App_Error init 	  (const Graphics_Init_Info& init_info);
	void 	  cleanup ();

	//TODO - internal result vs external result?
	//Result type that Graphics_Engine uses
	template <typename ResultType>
	using Result = engine_library::Result<ResultType, VkResult>;

	Result<Mesh> Register_Mesh (const Mesh_Data&);
	//TODO
	Result<Texture_Resources> Register_Texture(const Texture_Data&);
	App_Error Draw_Frame();
	//Camera_Matrix* Get_Camera_Matrix() {return static_cast<Camera_Matrix*>(scene_uniform_ptr);}
	//TODO
	void* Get_Scene_Uniform_Ptr () {return scene_uniform_ptr;}
	void* Get_Entity_Uniform_Ptr() {return per_entity_uniform_ptr;}

	struct{
		Scene main_scene;
	}temp;
private:
	using Scene = Graphics::Scene;
	template <typename T>
	using darray = engine_library::darray<T>;
	template <size_t N, typename T>
	using carray = engine_library::carray<N,T>;
	//Basic Vulkan Environment
	VkInstance 			vulkan_instance;//can forget about this until cleanup
	VkPhysicalDevice 	vulkan_physical_device;//TODO - can forget
	Vulkan_Device::Device	 vulkan_device;
	VkDebugReportCallbackEXT debug_callback;
	//Rendering Environment
	VkRenderPass 	 render_pass;
	VkPipeline		 graphics_pipeline;
	VkPipelineLayout pipeline_layout;
		//descriptor set layouts
	carray<3, VkDescriptorSetLayout> descriptor_set_layouts;
		//command queues that the engine will use to submit commands
		//todo - bundle these in a struct or something
		//maybe these should be part of the renderer
	VkQueue graphics_queue;
	VkQueue presentation_queue;
		//Command Pools TODO: transient vs longterm pool?
	VkCommandPool command_pool;
	//VkQueue transfer_queue; //wtf is a transfer command
		//shader program
	static constexpr uint32_t num_shader_stages = 2;
	static constexpr const char* vert_shader_file = "vert.spv";
	static constexpr const char* frag_shader_file = "frag.spv";
	//Framebuffer
	darray<VkFramebuffer> framebuffers;
		//Window + Surface
	VkSurfaceKHR window_surface;
		//Swapchain
	VkSwapchainKHR vulkan_swapchain;
	darray<VkImageView> swapchain_image_views;
		//Depth Buffer
	struct Depth_Buffer
	{
		VkImage 	   image;
		VkImageView    view;
		VkDeviceMemory memory;
	};
	Depth_Buffer depth_buffer;
	//Renderer Resources - put outside renderer?
		//Texture Sampler
	VkSampler texture_sampler;
		//Descriptor Sets
	static constexpr uint32_t max_textures = 10;
	VkDescriptorPool descriptor_pool;
		//Per-Entity Uniform
	static constexpr uint32_t max_entity_per_draw = 20;
	static constexpr uint32_t skybox_offset = 0u;//offset into entity_buffer for skybox
	VkBuffer per_entity_uniform_buffer;//TODO
	VkDeviceMemory per_entity_uniform_memory;//TODO
	void* per_entity_uniform_ptr;//TODO
	uint32_t per_entity_uniform_alignment;
	VkDescriptorSet	entity_descriptor;//TODO?
		//Scene Uniform
	VkBuffer scene_uniform_buffer;
	VkDeviceMemory scene_uniform_memory;
	void* scene_uniform_ptr;//TODO
	VkDescriptorSet scene_descriptor;//TODO: is this temp?
		//Command Buffers
	VkSemaphore image_available;
	VkSemaphore rendering_finished;
	VkCommandBuffer draw_buffer;

	//init funcs
	VkResult Create_Vulkan_Instance ();
	VkResult Create_Window_Surface	(Graphics_Init_Info::Window_Handle*);
	VkResult Setup_Debug_Callback	();
		//device creation
	uint32_t Select_Graphics_Queue_Family		();
	uint32_t Select_Presentation_Queue_Family	();
	VkPhysicalDevice Pick_Physical_Device 		();
	VkResult 		 Create_Vulkan_Device		();
		//swapchain creation
	VkResult Create_Swapchain ();
	VkSurfaceFormatKHR 	Choose_Swapchain_Surface_Format ();
	uint32_t 			Choose_Swapchain_Image_Count 	();
	VkExtent2D 			Choose_Swapchain_Extent			();
	VkPresentModeKHR 	Choose_Swapchain_Present_Mode 	();
		//renderpass creation
	VkAttachmentDescription Get_Color_Attachment_Description ();
	VkAttachmentDescription Get_Depth_Attachment_Description ();
	carray<2,VkAttachmentDescription> Get_Attachment_Descriptions ();
	VkResult Create_Render_Pass	();
		//graphics pipeline creation
	VkResult Register_Shader_Modules ();
	VkResult Create_Descriptor_Layouts ();
	VkResult Create_Pipeline_Layout	 ();
	VkResult Create_Graphics_Pipeline();
	carray<2,VkPipelineShaderStageCreateInfo>
		Get_Shader_Stages (const carray<2,VkShaderModule>);
	VkResult Create_Swapchain_Views();
		//depth buffer creation
	VkResult Create_Depth_Image	();
	VkResult Create_Depth_View	();
	VkResult Create_Depth_Memory();
	VkResult Transition_Depth_Layout();
	VkResult Create_Depth_Buffer();
		//framebuffer creation
	VkResult Create_Framebuffers();
		//command pool
	VkResult Create_Command_Pool ();
	VkResult Create_Semaphores	 ();
	VkResult Allocate_Draw_Buffer();
	//Renderer Resources
	Result<void*> Map_Whole_Memory(VkDeviceMemory);
	void	 Unmap_Memory(VkDeviceMemory);
	VkResult Init_Scene_Uniform();
	VkResult Create_Entity_Buffer();
	VkResult Init_Entity_Uniform();
	VkResult Create_Texture_Sampler();
	VkDeviceSize Get_Uniform_Alignment(size_t uniform_size);
	Result<VkBuffer> Create_Buffer_Handle(VkBufferUsageFlagBits, VkDeviceSize);
	Result<VkDeviceMemory> Alloc_Buffer_Memory(VkBuffer,VkMemoryPropertyFlags);
	Result<VkDeviceMemory> Alloc_Image_Memory(VkImage, VkMemoryPropertyFlags);
		//resource descriptors
	VkResult Create_Descriptor_Pool();
	Result<VkDescriptorSet> Create_Texture_Descriptor(VkImageView);
	VkResult Create_Entity_Descriptor();
	VkResult Create_Scene_Descriptor();
	VkResult Create_Descriptor_Sets();

	//Rendering Commands
	VkResult Generate_Draw_Commands(VkCommandBuffer, VkFramebuffer);
	void InsertCmd_Clear_Depth_Buffer(VkCommandBuffer);
	void InsertCmd_Begin_Frame	(VkCommandBuffer, const VkFramebuffer);
	void InsertCmd_Bind_Mesh 	(VkCommandBuffer, const Mesh_Draw_Info);
	void InsertCmd_Bind_Texture (VkCommandBuffer, const VkDescriptorSet);
	void InsertCmd_Draw_Scene 	(VkCommandBuffer, const Scene &);
	void InsertCmd_Draw_Skybox	(VkCommandBuffer,Mesh_Draw_Info,VkDescriptorSet);
	void InsertCmd_Draw_Entity_List(VkCommandBuffer,const Graphics::Entity_List&);
	void InsertCmd_Bind_EntityType (VkCommandBuffer,const Graphics::EntityType_Info&);

	//TODO - implement this throughout the code
	//TODO - #include shader_defines.h
	struct Descriptor_Set_Index{
		//TODO: constexpr vs enum?
		static constexpr auto Scene = 0u;
		static constexpr auto EntityType = 1u;
		static constexpr auto Entity = 2u;
//		enum{
//			Scene = 0,
//			EntityType = 1,
//			Entity = 2
//		};
	};

	//TODO: Choose optimal memoryTypeIndex for various buffers/images at init()
	//TODO: split this class up into Vulkan_Manager, Renderer
	//Vulkan Environment - setup stuff (Renderer_Environment?)
	//	-has definitions of device, graphics pipeline, etc
	//Renderer - decide what to draw, generate draw commands
	//Resource Manager - has containers for the vulkan handles that get created

	//TODO:
	//Result<VkImageView> Create_Image_View?
};


}//namespace Graphics
#endif
