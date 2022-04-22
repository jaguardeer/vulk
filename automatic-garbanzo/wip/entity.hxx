#ifndef ENTITY_HXX
#define ENTITY_HXX

class vulkan_mesh
{
	VkBuffer vertex_buffer;
	VkBuffer index_buffer;
	uint32_t num_indexes;
};

class shader_bindings
{
	VkDescriptorSet descriptor_set;
};

class entity_appearance
{
	vulkan_mesh 	_mesh;
	shader_bindings _bindings;
};

class entity_resources
{
	const char* mesh_file;
	const char* texture_file;
};

class entity_behaviour
{

};

class entity_type
{
	entity_appearance _appearance;
	entity_behaviour  _behaviour;
};

class entity_transform
{
public:
	void move();
	void rotate();
private:
	mat4* _transform;
}

class entity_instance
{
	entity_type 	 _type;
	entity_transform _transform;
};

void Fill_Render_Buffer()
{
	setup rendering
		begin command buffer
		bind pipeline
		begin render pass
	for(each entity type)
	{
		bind vertex_buffer
		bind index_buffer
		for(each entity instance)
		{
			bind descriptor_set
			vkCmdDrawIndexed(...);
		}
	}
	finish rendering
		end render pass
		end command buffer
}

#endif
