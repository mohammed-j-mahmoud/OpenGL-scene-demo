#version 330

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_normal;
layout (location = 2) in vec2 vertex_uv;
layout (location = 3) in vec4 vertex_color;
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

vec3 camera_position = vec3(0,0,3);
vec3 light_distance = vec3(1,1,1);

out vec4 vertex_color_out;

out vec3 v;
out vec3 n;
out vec3 l;
out vec3 h;

void main()
{

	vertex_color_out = vertex_color;
	gl_Position = projMat * viewMat * modelMat * vertex_position;

	l = normalize(light_distance);
	n = normalize((modelMat * normalize(vertex_normal)).xyz);
	v = normalize(camera_position - (modelMat*vertex_position).xyz);
	h = normalize(v + l);
}