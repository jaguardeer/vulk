#ifndef OBJ_LOADER
#define OBJ_LOADER


#include "obj.hxx"
#include "../renderer.hxx"
#include "../data_structs.hxx"
//engine_library::obj vs engine::mesh




//loads an obj file into renderer takes
struct OBJ_Loader
{
	using graphics_vertex = Renderer::vertex;
	using graphics_object = Graphics::Mesh_Data;
// Utility
	static bool isSameVertex (const graphics_vertex& g, const OBJ_File::Vertex& v, const OBJ_File& obj)
	{
		 return obj.vertex_positions[v.position_index] == g.position
		 && obj.tex_coords[v.texture_coord_index] == g.tex_coord;
	}



	static graphics_object load_graphics_object_from_obj_file(OBJ_File &obj)
	{
		size_t unique_count = 0;
		graphics_vertex* unique_vertexes = new graphics_vertex[obj.num_faces*3];//if every combo is unique this can hold it
		size_t index_count = 0;
		int32_t* indexes = new int32_t[obj.num_faces*3];
		//count and remember unique vertex attributes combos
		for(size_t f = 0; f < obj.num_faces; f++)
		for(size_t v = 0; v < 3; v++)
		{
			bool match = false;
			for(size_t u = 0; u < unique_count; u++)
				if(isSameVertex(unique_vertexes[u], obj.faces[f].vertexes[v], obj))
				{
					match = true;
					indexes[index_count++] = u;//if vertex matches then use that index
					break;
				};
			if(!match)
			{//if there was no match then save that vertex's values and its index
				const OBJ_File::Vertex vert = obj.faces[f].vertexes[v];

				indexes[index_count++] = unique_count;
				unique_vertexes[unique_count].position = obj.vertex_positions[vert.position_index];
				unique_vertexes[unique_count].tex_coord = obj.tex_coords[vert.texture_coord_index];
				unique_count++;
			}
		}

		graphics_object object;

		object.vertexes.resize(unique_count);
		for(size_t i = 0; i < unique_count; i++) object.vertexes[i] = unique_vertexes[i];
		object.indexes.resize(index_count);
		for(size_t i = 0; i < index_count; i++) object.indexes[i] = indexes[i];

		delete[] unique_vertexes;
		delete[] indexes;

		return object;
	}

};









#endif
