#version 300 es

uniform mat3 proj;
uniform vec4 color;
uniform mat3 tex_proj;
uniform mat3 tex2_proj;
in vec2 pos;
out vec4 v_color;
out vec2 v_texcoord;
out vec2 v_tex2coord;

void main() {
	vec3 pos3 = vec3(pos, 1.0);
	gl_Position = vec4(pos3 * proj, 1.0);
	v_color = color;
	v_texcoord = (pos3 * tex_proj).xy;
    v_tex2coord = (pos3 * tex2_proj).xy;
}
