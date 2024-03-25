#version 460

in vec3 v_position;
in vec3 v_normal;
in vec2 v_uv;

out vec4 f_frag_color;

uniform mat4 u_projection;
uniform mat4 u_modelview;
uniform mat3 u_normal;

uniform sampler2D u_texture;

void main() {
    f_frag_color = texture(u_texture, v_uv);
}
