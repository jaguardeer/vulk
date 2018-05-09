#include "graphics_engine.hxx"
#include <utility>
#include <iostream>
#include <cstring>
#include <windows.h>
#include "VkResult_to_CString.hxx"
#include "wip/bmp.hxx"
#include "wip/obj_loader.hxx"
#include "window_win32.hxx"
#include "input_win32.hxx"
#include "minmax.hxx"
using namespace std;
using engine_library::darray;
using Renderer::vertex;
using engine_library::BMP_File;
using Graphics::Mesh_Data;
using Graphics::Texture_Data;
using Graphics::Camera;

//MAJOR TODO: RESOURCE MANAGER

void OnMouseWheelRaw	(USHORT mouse_wheel_movement) 	{}
void OnKeyDown 			(WPARAM wparam) 				{}
void OnKeyUp 			(WPARAM wparam) 				{}
void OnMouseMove 		(LPARAM lparam) 				{}

//TODO: all this
struct{
	mat4* camera_matrix_ptr;//this isn't known until runtime if I want to directly use the mat4 in graphics memory
	vec3 camera_position;
	vec2 camera_orientation;//{pitch, yaw}
} global;
void Rotate_With_Mouse(mat4& orientation, const LONG delta_x, const LONG delta_y)
{
	constexpr vec3 yaw_vector = {0,1,0};
	constexpr vec3 pitch_vector = {1,0,0};
	//this way does rotation relative to current camera, spaceship style
	orientation *= mat4::rotation(delta_y, pitch_vector);
	orientation *= mat4::rotation(delta_x, yaw_vector);
}
void Generate_Camera_Matrix(mat4& view_matrix, mat4& skybox_matrix, vec3 position, vec2 orientation)
{
	constexpr vec3 yaw_vector = {0,1,0};
	constexpr vec3 pitch_vector = {1,0,0};
	view_matrix = mat4::rotation(orientation.y, pitch_vector) * mat4::rotation(-orientation.x, yaw_vector) * mat4::translation(position);
	skybox_matrix = mat4::translation(-position);
}
//TODO: just do Generate_View_Matrix
void OnMouseMoveRaw (LONG x_delta, LONG y_delta)
{
	//Rotate_With_Mouse(*camera_matrix_ptr, x_delta, y_delta);
	constexpr float mouse_sensitivity = 0.05;
	global.camera_orientation += vec2{x_delta, y_delta} * mouse_sensitivity;
}

constexpr float to_deg(float rad) { return rad*180./3.14; }
constexpr float to_rad(float deg) { return deg*3.14/180.; }

//TODO: Move_Distance_Along_Vector()
void Apply_WASD (vec3& position, float yaw, float time_delta)
{
	constexpr float move_speed = 0.005;
	const float move_factor = move_speed * time_delta;
	yaw = to_rad(yaw);

	using Input_Win32::Key_Is_Down;
	if(Key_Is_Down(0x57))
	{
		position.x -= move_factor * sinf(yaw);
		position.z -= move_factor * cosf(yaw);
	}
	if(Key_Is_Down(0x53))
	{
		position.x += move_factor * sinf(yaw);
		position.z += move_factor * cosf(yaw);
	}
	if(Key_Is_Down(0x41))
	{
		position.x += move_factor * cosf(yaw);
		position.z += move_factor * -sinf(yaw);
	}
	if(Key_Is_Down(0x44))
	{
		position.x -= move_factor * cosf(yaw);
		position.z -= move_factor * -sinf(yaw);
	}
}



constexpr vec3 FRONT_LEFT_BOTTOM  = {-1,  1,  1 };
constexpr vec3 FRONT_LEFT_TOP 	  = {-1, -1,  1 };
constexpr vec3 FRONT_RIGHT_BOTTOM = { 1,  1,  1 };
constexpr vec3 FRONT_RIGHT_TOP    = { 1, -1,  1 };
constexpr vec3 BACK_LEFT_BOTTOM   = {-1,  1, -1	};
constexpr vec3 BACK_LEFT_TOP 	  = {-1, -1, -1	};
constexpr vec3 BACK_RIGHT_BOTTOM  = { 1,  1, -1	};
constexpr vec3 BACK_RIGHT_TOP 	  = { 1, -1, -1	};

constexpr size_t num_cube_vertexes = 24;
const Renderer::vertex cube_vertexes[num_cube_vertexes] =
{
	//Front Face
	{FRONT_LEFT_BOTTOM ,{1/4.,1/3.}},
	{FRONT_RIGHT_BOTTOM,{2/4.,1/3.}},
	{FRONT_RIGHT_TOP   ,{2/4.,2/3.}},
	{FRONT_LEFT_TOP    ,{1/4.,2/3.}},
	//Right Face
	{FRONT_RIGHT_BOTTOM,{2/4.,1/3.}},
	{BACK_RIGHT_BOTTOM ,{3/4.,1/3.}},
	{BACK_RIGHT_TOP    ,{3/4.,2/3.}},
	{FRONT_RIGHT_TOP   ,{2/4.,2/3.}},
	//Back Face
	{BACK_RIGHT_BOTTOM ,{3/4.,1/3.}},
	{BACK_LEFT_BOTTOM  ,{4/4.,1/3.}},
	{BACK_LEFT_TOP     ,{4/4.,2/3.}},
	{BACK_RIGHT_TOP    ,{3/4.,2/3.}},
	//Left Face
	{BACK_LEFT_BOTTOM  ,{0/4.,1/3.}},
	{FRONT_LEFT_BOTTOM ,{1/4.,1/3.}},
	{FRONT_LEFT_TOP    ,{1/4.,2/3.}},
	{BACK_LEFT_TOP     ,{0/4.,2/3.}},
	//Top Face
	{FRONT_LEFT_TOP    ,{1/4.,2/3.}},
	{FRONT_RIGHT_TOP   ,{2/4.,2/3.}},
	{BACK_RIGHT_TOP    ,{2/4.,3/3.}},
	{BACK_LEFT_TOP     ,{1/4.,3/3.}},
	//Bottom Face
	{BACK_LEFT_BOTTOM  ,{1/4.,0/3.}},
	{BACK_RIGHT_BOTTOM ,{2/4.,0/3.}},
	{FRONT_RIGHT_BOTTOM,{2/4.,1/3.}},
	{FRONT_LEFT_BOTTOM ,{1/4.,1/3.}},
};

constexpr size_t num_cube_indexes = 36;
const uint16_t cube_indexes[num_cube_indexes] =
{
	 0, 1, 2,	 2, 3, 0, 	//Front
	 4, 5, 6,	 6, 7, 4, 	//Right
	 8, 9,10,	10,11, 8,	//Back
	12,13,14,	14,15,12, 	//Left
	16,17,18,	18,19,16, 	//Top
	20,21,22,	22,23,20, 	//Bottom
};

Mesh_Data generate_cube()
{
	Mesh_Data cube;
	cube.indexes  = darray<uint16_t>(num_cube_indexes);
	cube.vertexes = darray<vertex>  (num_cube_vertexes);
	memcpy(cube.indexes.data(), cube_indexes, sizeof(cube_indexes));
	memcpy(cube.vertexes.data(), cube_vertexes, sizeof(cube_vertexes));
	return cube;
}

Texture_Data load_texture(const char* const filename)
{
	BMP_File bmp;
	bmp.readFile(filename);
	return bmp.convertToTexture();
}

auto Load_Mesh_Data(const char* const filename)
{
	OBJ_File obj;
	obj.parseFile(filename);
	return OBJ_Loader::load_graphics_object_from_obj_file(obj);
}

auto Create_Game_Window()
{
	Graphics::Window_Create_Info window_info = {};
	window_info.title 	= "guccimane";
	window_info.width 	= 960;
	window_info.height 	= 960;
	const auto&& window_result = Graphics::window_win32::Create(window_info);
	if(window_result.error){
		cout << "Unable to make main window. Aborting" << endl;
		exit(1);
	}else return window_result.value;
}

#include <chrono>

int main()
{
	//init graphics
	cout << "Initializing graphics_engine...." << endl;
	Graphics::window_win32 game_window = Create_Game_Window(); 
	Graphics::Graphics_Engine graphics_engine;
	Graphics::Graphics_Init_Info init_info = {};
	auto window_handle = game_window.handle();
	init_info.window_handle = reinterpret_cast<Graphics::Graphics_Init_Info::Window_Handle*>(&window_handle);
	App_Error error = graphics_engine.init(init_info);
	if(!error) cout << "Init completed with no errors" << endl;
	else
	{
		cout << "Failed to init graphics engine. Print errors to console:"
			 << endl << error.error_msg;
		cout << "--- crash imminent ---" << endl;
	}

	//register some meshes and textures
	cout << "Loading skybox_mesh..." << flush;
	const auto&& skybox_mesh_data = generate_cube();
	auto [skybox_mesh,e1] = graphics_engine.Register_Mesh(skybox_mesh_data);
	if(e1) cout << "failed: " << VkResult_to_CString(e1) << endl;
	else   cout << "success" << endl;

	cout << "Loading skybox_texture..." << flush;
	const auto&& skybox_texture_data = load_texture("skybox.bmp");
	auto [skybox_texture,e2] = graphics_engine.Register_Texture(skybox_texture_data);
	if(e2) cout << "failed: " << VkResult_to_CString(e2) << endl;
	else   cout << "success" << endl;

	cout << "Loading monkey_mesh..." << flush;
	const auto&& monkey_mesh_data = Load_Mesh_Data("monkey.obj");
	const auto&& monkey_mesh_result = graphics_engine.Register_Mesh(monkey_mesh_data);
	if(auto&& e = monkey_mesh_result.error){
		  cout << "failed: " << VkResult_to_CString(e) << endl;
	}else cout << "success" << endl;

	cout << "Loading monkey_texture..." << flush;
	const auto&& monkey_texture_data = load_texture("monkey.bmp");
	auto [monkey_texture, e3] = graphics_engine.Register_Texture(monkey_texture_data);
	if(e3) cout << "failed: " << VkResult_to_CString(e2) << endl;
	else   cout << "success" << endl;

	//Setup scene
	Graphics::Scene& main_scene = graphics_engine.temp.main_scene;
	main_scene.skybox_mesh = skybox_mesh.draw_info;
	main_scene.skybox_texture_descriptor = skybox_texture.descriptor;
	main_scene.entity_lists.resize(1);
	main_scene.entity_lists[0].entity_type.texture_descriptor = monkey_texture.descriptor;
	main_scene.entity_lists[0].entity_type.mesh = monkey_mesh_result.value.draw_info;
	main_scene.entity_lists[0].entities.resize(2);
	main_scene.entity_lists[0].entities[0].offset = sizeof(Entity_Uniform);
	main_scene.entity_lists[0].entities[1].offset = 2*sizeof(Entity_Uniform);
	Entity_Uniform* entity_uniforms = static_cast<Entity_Uniform*>(graphics_engine.Get_Entity_Uniform_Ptr());
	//Camera
	Camera& camera = *static_cast<Camera*>(graphics_engine.Get_Scene_Uniform_Ptr());
	using time = chrono::high_resolution_clock;
	const auto start_time = time::now();
	camera.matrix.proj = mat4::perspective_infinite(90, 1);//TODO: perspective seems off
	global.camera_matrix_ptr = &camera.matrix.view;


	//TODO
	Input_Win32::Register_RawInput();
	game_window.show();

	//fire away
	auto num_frames = 0u;
	while(--num_frames)
	{
		Input_Win32::Process_Message_Queue();
		const chrono::duration<float> time_diff = time::now() - start_time;
		const auto t = time_diff.count();
		Apply_WASD(global.camera_position, global.camera_orientation.x, t);
		Generate_Camera_Matrix(
			camera.matrix.view,
			entity_uniforms[0].matrix,
			global.camera_position,
			global.camera_orientation
		);
		if(Input_Win32::Key_Is_Down(VK_ESCAPE)) exit(0);
		const auto&& rotation = mat4::rotation(time_diff.count()*12, {0,0,1});
		//camera.matrix.view = rotation * mat4::scale(0.5);
		entity_uniforms[1].matrix = mat4::rotation(10.0*t,{1,1,1}) * mat4::translation(0, 0, 5) * mat4::scale(0.5) * mat4::rotation(60*t,{0,1,1});
		entity_uniforms[2].matrix = mat4::translation(0,0,5) * mat4::rotation(360*t, {1,1,1});
		error = graphics_engine.Draw_Frame();
		if(error)
		{
			cout << error.error_msg << endl;
			return 9;
		}
		Sleep(1);//TODO: Use wait functions to wait for frame or something
				 //		 or have vkAcquireImage block until frame is available?
	}

	cout << "Cleaning up graphics_engine..." << endl;
	graphics_engine.cleanup();
	cout << "Done." << endl;
	return 0;
}
