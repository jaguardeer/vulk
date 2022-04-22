#ifndef OBJ_FILE_HPP
#define OBJ_FILE_HPP
#include "text_segment.hxx"
#include "../vec.hxx"

//file blob -> lines -> either<vertex, texture coord, face ....> -> graphics_vertex[] and vertex_indexes[]
//line : <type> <field1> <field2> .....


//see https://en.wikipedia.org/wiki/Wavefront_.obj_file
class OBJ_File
{
	public:
		//TODO
	void parseFile(const char* filename)
	{
		Text_Blob obj_text;
		obj_text.slurp_file(filename);
		//count stuff
		obj_text.forEachLine([this](const char* start, const char* end)
		{	
			const Text_Segment line = {start,end};
			const Text_Segment type = line.word(0);
			if(type == "v") num_vertex_positions++;
			if(type == "vt") num_tex_coords++;
			if(type == "f") num_faces++;
		});
		//make room
		if(num_vertex_positions > 0) vertex_positions = new Vertex_Position[num_vertex_positions];
		else std::cout << "obj.hxx: num_vertex_positions <= 0" << endl;
		if(num_tex_coords > 0) tex_coords = new Texture_Coord[num_tex_coords];
		else std::cout << "obj.hxx: num_tex_coords <= 0" << endl;
		if(num_faces > 0) faces = new Face[num_faces];
		else std::cout << "obj.hxx: num_faces <= 0" << endl;

		//fill structs
		obj_text.forEachLine([this](const char* start, const char* end)
		{
			parseLine(start,end);
		});
	}




	//private:
//List of geometric vertices, with (x,y,z[,w]) coordinates, w is optional and defaults to 1.0
	typedef vec3 Vertex_Position;

	uint32_t num_vertex_positions = 0;
	Vertex_Position* vertex_positions;

//List of texture coordinates, in (u, v [,w]) coordinates, these will vary between 0 and 1, w is optional and defaults to 0.
	typedef vec2 Texture_Coord;

	uint32_t num_tex_coords = 0;
	Texture_Coord* tex_coords;

//A valid vertex index matches the corresponding vertex elements of a previously defined vertex list.
//If an index is positive then it refers to the offset in that vertex list, starting at 1.
//If an index is negative then it relatively refers to the end of the vertex list, -1 referring to the last element.
	struct Vertex
	{
		int32_t position_index;
		int32_t texture_coord_index;
	};

//Faces are defined using lists of vertex, texture and normal indices.
//Polygons such as quadrilaterals can be defined by using more than three vertex/texture/normal indices.
	struct Face
	{
		Vertex vertexes[3];
	};

	uint32_t num_faces = 0;
	Face* faces;





//	Text processing
	void parseVertexPosition(const Text_Segment& line)
	{
		static size_t parse_count = 0;

		const float x = line.word(1).parseAsFloat();
		const float y = line.word(2).parseAsFloat();
		const float z = line.word(3).parseAsFloat();

		const Vertex_Position pos = {x,y,z};
		vertex_positions[parse_count] = pos;
		parse_count++;
	}

	void parseTextureCoord(const Text_Segment& line)
	{
		static size_t parse_count = 0;

		const float u = line.word(1).parseAsFloat();
		const float v = line.word(2).parseAsFloat();

		const Texture_Coord tex_coord = {u,v};
		tex_coords[parse_count] = tex_coord;
		parse_count++;
	}

	void parseFace(const Text_Segment& line)
	{
		static size_t parse_count = 0;
		Vertex v[3];
		for(size_t i=0; i < 3; i++)
		{
			const Text_Segment pos_text = line.word(i+1);
			const char* tex_coord_text = pos_text.gotoChar('/')+1;
			//this will break very easily - make tocstr() or limited atoi
			v[i].position_index = pos_text.parseAsInt()-1;
			v[i].texture_coord_index = atoi(tex_coord_text)-1;
		}
		const Face f = {{v[0],v[1],v[2]}};
		faces[parse_count] = f;
		parse_count++;
	}

	//handle lines based on word(0)
	//special case if word(0)[0] == '#' for comment
	using Line_Parsing_Function = void (OBJ_File::*)(const Text_Segment& line);
	void parseLine(const char* start, const char* end)
	{
		struct {
			const char* id;
			Line_Parsing_Function handleLine;
		}const Line_Handlers[] = {
			{"v",parseVertexPosition},
			{"vt",parseTextureCoord},
			{"f",parseFace}
		};

		const Text_Segment line = {start,end};
		const Text_Segment arg = line.word(0);
		if(arg.start[0] == '#') return;//lines starting with # are comments
		else for (auto i : Line_Handlers)
		if(arg == i.id)
		{
			(this->*i.handleLine)(line);
			return;
		}
	}

};




#endif
/*


# 
v ...
...
# List of 
vt 0.500 1 [0]
vt ...
...
# List of vertex normals in (x,y,z) form; normals might not be unit vectors.
vn 0.707 0.000 0.707
vn ...
...
# Parameter space vertices in ( u [,v] [,w] ) form; free form geometry statement ( see below )
vp 0.310000 3.210000 2.100000
vp ...
...
# Polygonal face element (see below)
f 1 2 3
f 3/1 4/2 5/3
f 6/4/1 3/5/3 7/6/5
f 7//1 8//2 9//3
f ...
...
# Line element (see below)
l 5 8 1 2 4 9

*/
