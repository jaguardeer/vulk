#version 450
#extension GL_ARB_separate_shader_objects : enable //required for vulkan?
#extension GL_GOOGLE_include_directive : enable
#include "shader_defines.h"
//TODO: descriptor set
//TODO: use header to make bindings and structs obey ODR

layout(set = 0, binding = 0) uniform Scene_Uniform
{
	Camera_Matrix camera;
} scene;

layout(set = 2, binding = 0) uniform Per_Entity_Uniform
{
	Entity_Uniform u;
} entity;//TODO: make this entity[max_entities_of_single_type]
		 //or, stream the modelView in as a vertex attribute

const vec4 testVerts[4] =
{
	vec4(-1.00,  1.00, 0.5, 1.0),
	vec4(-1.00, -1.00, 0.5, 1.0),
	vec4( 1.00, -1.00, 0.5, 1.0),
	vec4( 1.00,  1.00, 0.5, 1.0),
};

const vec2 testTexCoord[4] =
{
	vec2( 0.00, 0.00),
	vec2( 0.00, 1.00),
	vec2( 1.00, 1.00),
	vec2( 1.00, 0.00),
};

//VkVertexInputBindingDescription
//VkVertexInputAttributeDescription
layout(location = 0) in vec3 in_xyz;
layout(location = 1) in vec2 in_uv;
//layout(location = 2) in vec3 in_rgb;

layout(location = 0) out vec2 frag_uv;
//layout(location = 1) out vec3 frag_rgb;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
	//gl_Position= vec4(in_xyz, 1);
    //gl_Position = scene.camera.proj * vec4(in_xyz,1);
    //gl_Position = scene.camera.view * vec4(in_xyz,1);
    //gl_Position = scene.camera.proj * scene.camera.view * vec4(in_xyz,1);
    gl_Position = scene.camera.proj * scene.camera.view * entity.u.matrix * vec4(in_xyz,1);
	frag_uv = in_uv;
    //frag_uv  = testTexCoord[gl_VertexIndex];
    //gl_Position = testVerts[gl_VertexIndex];
    //frag_rgb = in_rgb;
}

