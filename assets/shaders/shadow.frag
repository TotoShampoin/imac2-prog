#version 460

in vec2 v_uv;

out vec4 f_frag_color;

uniform vec3 u_color;

void main() {
    vec2 uv = v_uv * 2.0 - 1.0;
    float d = length(uv);
    f_frag_color = vec4(u_color, d < 1.0 ? .66666 : 0.0);
}
