#ifndef SHADER_DEFINES_H
#define SHADER_DEFINES_H

struct Camera_Matrix
{
	mat4 view;
	mat4 proj;
};

struct Entity_Uniform
{
	mat4 matrix;
};

#endif