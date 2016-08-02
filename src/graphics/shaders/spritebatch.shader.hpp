#pragma once

#include <string>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

inline std::string
SpriteBatchVertexShaderSource (void)
{
return R"(#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in int tid;
layout (location = 3) in vec4 color;

uniform mat4 pr_matrix;
uniform mat4 vw_matrix = mat4(1.0);
uniform mat4 ml_matrix = mat4(1.0);

out DATA
{
	vec4 position;
    vec2 uv;
    flat int tid;
	vec4 color;
} vs_out;

void main()
{
	gl_Position = pr_matrix * vw_matrix * ml_matrix * position;
	vs_out.position = ml_matrix * position;
    vs_out.uv = uv;
    vs_out.tid = tid;
	vs_out.color = color;
})";
}

inline std::string
SpriteBatchFragmentShaderSource (void)
{
return R"(#version 330 core

layout (location = 0) out vec4 color;

uniform vec2 light_pos;

in DATA
{
	vec4 position;
    vec2 uv;
    flat int tid;
	vec4 color;
} fs_in;

uniform sampler2D textures[16];

void main()
{
	//float intensity = 1.0 / length(fs_in.position.xy - light_pos);
    vec4 texColor = fs_in.color;
    if (fs_in.tid >= 0)
    {
        texColor = fs_in.color.a * texture(textures[fs_in.tid], fs_in.uv);
    }

    color = texColor;// * intensity;
})";
}

} // namespace Graphics
} // namespace RDGE
