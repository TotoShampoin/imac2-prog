#version 460

in vec2 v_uv;

out vec4 f_frag_color;

uniform sampler2D u_texture;
uniform sampler2D u_texture_blend;
uniform float u_time;

void main() {
    float noise_value = texture(u_texture, mod(v_uv + u_time, vec2(1))).r;
    vec3 color = texture(u_texture_blend, vec2(noise_value, 0)).rgb;

    f_frag_color = vec4(color, (noise_value > 0.45 && noise_value < 0.55) ? (noise_value * 1.5f) : (noise_value / 2.f));
}
