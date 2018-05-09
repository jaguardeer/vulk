#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 frag_uv;
//layout(location = 1) in vec3 fragColor;

layout(set = 1, binding = 0) uniform sampler2D texture_sampler;

layout(location = 0) out vec4 pixel_color;

void main()
{
	pixel_color = texture(texture_sampler, frag_uv);
	//pixel_color = vec4(1.0, 1.0, 0.0, 1.0);
}
