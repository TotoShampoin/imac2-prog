#version 460

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_uv;

uniform mat4 u_projection;
uniform mat4 u_modelview;
uniform mat3 u_normal;

void main() {
    vec4 position = u_modelview * vec4(a_position, 1.);
    vec3 normal = u_normal * a_normal;

    v_position = position.xyz;
    v_normal = normal;
    v_uv = a_uv;

    gl_Position = u_projection * position;
}
