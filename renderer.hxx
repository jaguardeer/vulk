#ifndef RENDERER_HXX
#define RENDERER_HXX
extern "C"{
#include <vulkan/vulkan.h>
}
#include "vec.hxx"
namespace Renderer{

//TODO: should this be Graphics::vertex?
struct vertex
{
	vec3 position;//TODO: rename xyz
	vec2 tex_coord;//TODO: rename uv
};

//Vertex Input Description
//TODO - use shader_defines.h and put this in shader_description.hxx
constexpr VkVertexInputBindingDescription vertex_binding_description =
{
	0,							//binding
	sizeof(vertex),		//stride
	VK_VERTEX_INPUT_RATE_VERTEX //inputRate
};

constexpr VkVertexInputAttributeDescription position_attribute =
{
	0,									//location
	0,									//binding
	VK_FORMAT_R32G32B32_SFLOAT,			//format
	offsetof(vertex, position)	//offset
};

constexpr VkVertexInputAttributeDescription tex_coord_attribute =
{
	1,									//location
	0,									//binding
	VK_FORMAT_R32G32_SFLOAT,			//format
	offsetof(vertex, tex_coord)	//offset
};

constexpr VkVertexInputAttributeDescription vertex_attribute_descriptions[2] =
{
	position_attribute,
	tex_coord_attribute
};

constexpr VkPipelineVertexInputStateCreateInfo vertex_input_description =
{
	VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,	//sType
	nullptr,													//pNext
	0x0,														//flags
	1,															//num bindings
	&vertex_binding_description,								//pBindings
	2,															//num attributes
	vertex_attribute_descriptions								//pAttributes
};

}//namespace Renderer
#endif
/*
class renderer
{
public:
	App_Error init();
	//Rendering Environment
	VkRenderPass 	 render_pass;
	VkPipelineLayout pipeline_layout;
	VkPipeline 		 graphics_pipeline;

	VkResult Create_Graphics_Pipeline();
};
*/
