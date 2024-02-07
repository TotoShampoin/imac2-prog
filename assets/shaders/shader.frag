#version 460

in vec3 v_position;
in vec3 v_normal;
in vec2 v_uv;

out vec4 f_frag_color;

uniform mat4 u_projection;
uniform mat4 u_modelview;
uniform mat3 u_normal;

uniform vec4 u_color;

void main() {
    f_frag_color = u_color * max(0, dot(v_normal, vec3(0,0,1)));
}
