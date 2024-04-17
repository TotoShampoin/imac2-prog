#version 460

in vec2 v_uv;

out vec4 f_frag_color;

uniform sampler2D u_texture;
uniform sampler2D u_texture_blend;
uniform float u_time;

const float range = .01;

void main() {
    float noise_1 = texture(u_texture, mod(v_uv - u_time, vec2(1))).r;
    float noise_2 = texture(u_texture, mod(v_uv * .5 + u_time, vec2(1))).r;

    float noise_value = (noise_1 + noise_2) / 2.f;

    vec3 color = texture(u_texture_blend, vec2(noise_value, 0)).rgb;

    f_frag_color = vec4(color, (noise_value > 0.5 - range && noise_value < 0.5 + range) ? .5f : 0.f);
}
