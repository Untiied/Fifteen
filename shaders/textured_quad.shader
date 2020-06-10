#version 330 core
layout (location = 0) in vec4 vertex_information;

uniform mat4 model;
uniform mat4 projection;

uniform vec2 size;
uniform vec3 color;

out vec2 texture_pos;
out vec3 tex_color;
void main(){
    gl_Position = projection * model * vec4(vec2(vertex_information.x * size.x, vertex_information.y * size.y), 0.0, 1.0);
    texture_pos = vertex_information.zw;
    tex_color = color;
}


--fragment
#version 330 core

in vec2 texture_pos;
in vec3 tex_color;

out vec4 color;

uniform sampler2D texture_sample;

void main()
{
    color = vec4(tex_color, 1.0) * texture(texture_sample, texture_pos);
}